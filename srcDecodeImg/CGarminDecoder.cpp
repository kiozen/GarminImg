/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "CGarminDecoder.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <stdarg.h>
#include <stdio.h>

#include "CException.h"
#include "CGarminPolygon.h"
#include "CGarminRaster.h"
#include "CGarminStrTbl6.h"
#include "CGarminStrTbl8.h"
#include "CGarminStrTblUtf8.h"
#include "CSubfileTask.h"

extern QCommandLineParser allArgs;

static const char* gtiffArgs[] = {"COMPRESS=JPEG", "TILED=YES",
                                  "BLOCKXSIZE=1024", "BLOCKYSIZE=1024", NULL};

static void minno(hdr_tre_t* trehdr, QByteArray& data) {
  if (trehdr->flag & 0x80) {
    quint32 nlevels = trehdr->tre1_size / sizeof(tre_1_t);

    quint8 key[5];
    quint8 tbl[trehdr->tre1_size];
    memcpy(tbl, data.data(), trehdr->tre1_size);

    key[0] = (((tbl[0] >> 4) + 8) & 0x0F);
    key[1] = (((tbl[3] >> 4) + 16) & 0x0F);
    key[2] = (((tbl[3] & 0x0F) + 16) & 0x0F);
    key[3] = ((tbl[4] >> 4) & 0x7);
    if (nlevels > 2) {
      key[3] ^= (((tbl[9] >> 4) + 16 - key[3]) & 0x08);
    }
    key[4] = (((tbl[2] >> 4) + 16 - 0) & 15);

    for (quint32 i = 0; i < nlevels * 4; i++) {
      tbl[i] = (((((tbl[i] >> 4) + 16 - key[(i * 2) % 5]) & 15) << 4) +
                ((((tbl[i] & 15) + 16 - key[(i * 2 + 1) % 5]) & 15)));
    }

    tre_1_t* ml = (tre_1_t*)tbl;
    for (quint32 i = 0; i < nlevels; i++) {
      ++ml;
    }

    memcpy(data.data(), tbl, trehdr->tre1_size);
    trehdr->flag &= ~0x80;
  }
}

CGarminDecoder::CGarminDecoder(const QString& filename)
    : filename(filename), basename(QFileInfo(filename).baseName()) {
  drvman = GetGDALDriverManager();
  driver = drvman->GetDriverByName("GTiff");
}

void CGarminDecoder::print(const char* format, ...) {
  QMutexLocker lock(&mutex);
  va_list args;
  va_start(args, format);
  vfprintf(stdout, format, args);
  va_end(args);

  fflush(stdout);
}

void CGarminDecoder::loadBasics() {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw CException("failed to open file: " + filename);
  }

  readFAT(file);
  readSubfiles(file);
}

void CGarminDecoder::readFAT(QFile& file) {
  subfiles.clear();
  file.seek(0);

  gmapsupp_imghdr_t imghdr;
  file.read((char*)&imghdr, sizeof(gmapsupp_imghdr_t));
  file.seek(imghdr.offsetFAT * 0x200);

  print("---- Header --- size(%08X)\n", sizeof(gmapsupp_imghdr_t));
  imghdr.print();

  size_t blocksize = imghdr.blocksize();

  print("---- FAT --- \n");
  FATBlock_t FATBlock;
  file.read((char*)&FATBlock, sizeof(FATBlock_t));
  while (FATBlock.flag == 1) {
    if (file.atEnd()) {
      throw CException("premature end of file.");
    }

    char tmpstr[64] = {0};
    memcpy(tmpstr, FATBlock.name,
           sizeof(FATBlock.name) + sizeof(FATBlock.type));
    tmpstr[sizeof(FATBlock.name) + sizeof(FATBlock.type)] = 0;

    if (FATBlock.size != 0 &&
        !subfiles.contains(tmpstr) /*&& tmpstr[0] != 0x20*/) {
      subfile_t& subfile = subfiles[tmpstr];
      subfile.name = tmpstr;

      memcpy(tmpstr, FATBlock.type, sizeof(FATBlock.type));
      tmpstr[sizeof(FATBlock.type)] = 0;

      subfile_part_t& part = subfile.parts[tmpstr];
      part.size = FATBlock.size;
      part.offset = FATBlock.blocks[0] * blocksize;

      print("%s %s %08X %08X %02X\n", subfile.name.toLatin1().data(), tmpstr,
            part.offset, part.size, FATBlock.part);
    }

    file.read((char*)&FATBlock, sizeof(FATBlock_t));
  }
}

void CGarminDecoder::readSubfiles(QFile& file) {
  foreach (const QString& key, subfiles.keys()) {
    if (key.mid(8) == "GMP") {
      readGMP(file, subfiles[key]);
    } else if (key.mid(8) == "MPS") {
      readMPS(file, subfiles[key]);
    } else if (key.mid(8) == "TRE") {
      readTRE(file, subfiles[key]);
    } else if (key.mid(8) == "RGN") {
      readRGN(file, subfiles[key]);
    } else if (key.mid(8) == "LBL") {
      readLBL(file, subfiles[key]);
    }
  }

  fflush(stdout);
  fflush(stderr);
}

void CGarminDecoder::readTRE(QFile& file, subfile_t& subfile) {
  printf("   --- TRE header ---\n");
  file.seek(subfile.parts["TRE"].offset);
  quint16 size;
  file.read((char*)&size, sizeof(size));

  file.seek(subfile.parts["TRE"].offset);
  file.read((char*)&subfile.hdrTRE, size);
  subfile.hdrTRE.print(subfile.parts["TRE"].offset);
}

void CGarminDecoder::readRGN(QFile& file, subfile_t& subfile) {
  printf("   --- RGN header ---\n");
  file.seek(subfile.parts["RGN"].offset);
  quint16 size;
  file.read((char*)&size, sizeof(size));

  file.seek(subfile.parts["RGN"].offset);
  file.read((char*)&subfile.hdrRGN, size);
  subfile.hdrRGN.print(subfile.parts["RGN"].offset);
}

void CGarminDecoder::readLBL(QFile& file, subfile_t& subfile) {
  printf("   --- LBL header ---\n");
  file.seek(subfile.parts["LBL"].offset);
  quint16 size;
  file.read((char*)&size, sizeof(size));

  file.seek(subfile.parts["LBL"].offset);
  file.read((char*)&subfile.hdrLBL, size);
  subfile.hdrLBL.print(subfile.parts["LBL"].offset);
}

void CGarminDecoder::readMPS(QFile& file, subfile_t& subfile) {
  print("--- Map Info ---\n");
  file.seek(subfile.parts["MPS"].offset);

  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::LittleEndian);

  quint8 type;
  quint16 length;

  stream >> type >> length;
  while (type != 0) {
    switch (type) {
      case 0x46:
        readProductInfo(stream);
        break;

      case 0x4c:
        readMapInfo(stream);
        break;

      default:
        stream.skipRawData(length);
    }

    stream >> type >> length;
  }
}

QString CGarminDecoder::readRawString(QDataStream& stream) {
  QByteArray label;
  quint8 tmp;
  stream >> tmp;
  while (tmp != 0) {
    label.append(tmp);
    stream >> tmp;
  }
  return QString::fromUtf8(label);
}

void CGarminDecoder::readProductInfo(QDataStream& stream) {
  quint16 idProd;
  quint16 idFamily;
  stream >> idProd >> idFamily;
  print("Product Info: %i %i %s\n", idProd, idFamily,
        readRawString(stream).toUtf8().data());
}

void CGarminDecoder::readMapInfo(QDataStream& stream) {
  quint16 idProd;
  quint16 idFamily;
  quint32 mapNumber;

  stream >> idProd >> idFamily >> mapNumber;
  const QString& seriesName = readRawString(stream);
  const QString& mapDesc = readRawString(stream);
  const QString& areaName = readRawString(stream);
  quint32 mapId;
  quint32 dummy;

  stream >> mapId >> dummy;

  print("Map Info: %i %i %i %s %s %s %08X %i \n", idProd, idFamily, mapNumber,
        seriesName.toLocal8Bit().data(), mapDesc.toLocal8Bit().data(),
        areaName.toLocal8Bit().data(), mapId, mapId);
}

void CGarminDecoder::readGMP(QFile& file, subfile_t& subfile) {
  file.seek(subfile.parts["GMP"].offset);

  print("--- GMP Header %s (%08X)---\n", subfile.name.toLatin1().data(),
        (quint32)file.pos());

  gmp_hdr_t hdr;
  file.read((char*)&hdr, sizeof(hdr));
  hdr.print();
  QString copyright(file.readLine());

  if (hdr.offsetTRE) {
    printf("   --- TRE header ---\n");
    file.seek(subfile.parts["GMP"].offset + hdr.offsetTRE);
    file.read((char*)&subfile.hdrTRE, sizeof(subfile.hdrTRE));
    subfile.hdrTRE.print(subfile.parts["GMP"].offset);
  }
  if (hdr.offsetRGN) {
    printf("   --- RGN header ---\n");
    file.seek(subfile.parts["GMP"].offset + hdr.offsetRGN);
    file.read((char*)&subfile.hdrRGN, sizeof(subfile.hdrRGN));
    subfile.hdrRGN.print(subfile.parts["GMP"].offset);
  }
  if (hdr.offsetLBL) {
    printf("   --- LBL header ---\n");
    file.seek(subfile.parts["GMP"].offset + hdr.offsetLBL);
    file.read((char*)&subfile.hdrLBL, sizeof(subfile.hdrLBL));
    subfile.hdrLBL.print(subfile.parts["GMP"].offset);
  }
  if (hdr.offsetNOD) {
    printf("   --- NOD header ---\n");
    file.seek(subfile.parts["GMP"].offset + hdr.offsetNOD);
    file.read((char*)&subfile.hdrNOD, sizeof(subfile.hdrNOD));
    subfile.hdrNOD.print();
  }
  if (hdr.offsetNET) {
    printf("   --- NET header ---\n");
    file.seek(subfile.parts["GMP"].offset + hdr.offsetNET);
    file.read((char*)&subfile.hdrNET, sizeof(subfile.hdrNET));
    subfile.hdrNET.print();
  }
  if (hdr.offsetDEM) {
    printf("   --- DEM header ---\n");
    file.seek(subfile.parts["GMP"].offset + hdr.offsetDEM);
    file.read((char*)&subfile.hdrDEM, sizeof(subfile.hdrDEM));
    subfile.hdrDEM.print(subfile.parts["GMP"].offset);
  }

  printf("   --- Analysis of map data ---\n");
  readMapLevels(file, subfile);
  readSubdivInfo(file, subfile);
  readSubdivInfoExt(file, subfile);
  readTre8(file, subfile);

  for (const subdiv_t& subdiv : subfile.subdivs) {
    subdiv.print();
  }

  readStringTable(file, subfile);
}

void CGarminDecoder::readMapLevels(QFile& file, subfile_t& subfile) {
  // read map levels from TRE1
  file.seek(subfile.parts["GMP"].offset + subfile.hdrTRE.tre1_offset);
  QByteArray bufMapLevels = file.read(subfile.hdrTRE.tre1_size);

  if (subfile.hdrTRE.flag & 0x80) {
    minno(&subfile.hdrTRE, bufMapLevels);
  }

  quint32 nLevels = subfile.hdrTRE.tre1_size / sizeof(tre_1_t);
  quint32 nSubdivs = 0;
  quint32 nSubdivsLast = 0;

  tre_1_t* pMapLevel = (tre_1_t*)bufMapLevels.data();
  for (quint32 i = 0; i < nLevels; i++) {
    nSubdivs += pMapLevel->subdiv;
    nSubdivsLast = pMapLevel->subdiv;

    maplevel_t ml;
    ml.inherit = pMapLevel->zoom & 0x80;
    ml.zoom = pMapLevel->zoom & 0x7F;
    ml.bits = pMapLevel->bits;
    ml.subdiv = pMapLevel->subdiv;

    subfile.mapLevels << ml;
    pMapLevel->print();
    pMapLevel++;
  }

  subfile.nSubdivsNext = nSubdivs - nSubdivsLast;
  // resize number of sub-divisions
  subfile.subdivs.resize(nSubdivs);
}

void CGarminDecoder::readSubdivInfo(QFile& file, subfile_t& subfile) {
  file.seek(subfile.parts["GMP"].offset + subfile.hdrTRE.tre2_offset);
  QByteArray tre2 = file.read(subfile.hdrTRE.tre2_size);

  tre_2_next_t* pTre2N = (tre_2_next_t*)tre2.data();

  QVector<subdiv_t>::iterator subdiv = subfile.subdivs.begin();
  QVector<subdiv_t>::iterator subdiv_prev = subfile.subdivs.end();

  int mapLevelIdx = 0;
  quint32 nSubdiv = subfile.mapLevels[mapLevelIdx].subdiv;
  // parse all 16 byte subdivision entries
  quint32 l;
  for (l = 0; l < subfile.nSubdivsNext; ++l, --nSubdiv) {
    qint32 cx, cy;
    qint32 width, height;

    subdiv->maplevel = &subfile.mapLevels[mapLevelIdx];
    subdiv->n = l + 1;
    subdiv->next = pTre2N->next;
    subdiv->terminate = TRE_SUBDIV_TERM(pTre2N);
    subdiv->rgn_start = pTre2N->rgn_offset[0] | pTre2N->rgn_offset[1] << 8 |
                        pTre2N->rgn_offset[2] << 16;

    // skip if this is the first entry
    if (subdiv_prev != subfile.subdivs.end()) {
      subdiv_prev->rgn_end = subdiv->rgn_start;
    }

    subdiv->hasPoints = pTre2N->elements & 0x10;
    subdiv->hasIdxPoints = pTre2N->elements & 0x20;
    subdiv->hasPolylines = pTre2N->elements & 0x40;
    subdiv->hasPolygons = pTre2N->elements & 0x80;

    // if all subdivisions of this level have been parsed, switch to the next
    // one
    if (nSubdiv == 0) {
      mapLevelIdx++;
      subdiv->maplevel = &subfile.mapLevels[mapLevelIdx];
      nSubdiv = subdiv->maplevel->subdiv;
    }

    subdiv->level = subdiv->maplevel->zoom;
    subdiv->shift = 24 - subdiv->maplevel->bits;

    cx = pTre2N->center_lng[0] | pTre2N->center_lng[1] << 8 |
         pTre2N->center_lng[2] << 16;
    subdiv->iCenterLng = cx;
    cy = pTre2N->center_lat[0] | pTre2N->center_lat[1] << 8 |
         pTre2N->center_lat[2] << 16;
    subdiv->iCenterLat = cy;
    width = TRE_SUBDIV_WIDTH(pTre2N) << subdiv->shift;
    height = pTre2N->height << subdiv->shift;

    subdiv->north = GARMIN_RAD(cy + height + 1);
    subdiv->south = GARMIN_RAD(cy - height);
    subdiv->east = GARMIN_RAD(cx + width + 1);
    subdiv->west = GARMIN_RAD(cx - width);

    subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north),
                          QPointF(subdiv->east, subdiv->south));

    subdiv_prev = subdiv;
    ++pTre2N;
    ++subdiv;
  }

  // the subdivisions of the last zoom level do not have a `next` field.
  quint32 nSubdivs = subfile.subdivs.size();
  mapLevelIdx++;
  tre_2_t* pTre2L = pTre2N;

  for (; l < nSubdivs; ++l) {
    qint32 cx, cy;
    qint32 width, height;
    subdiv->maplevel = &subfile.mapLevels[mapLevelIdx];
    subdiv->n = l + 1;
    subdiv->next = 0;
    subdiv->terminate = TRE_SUBDIV_TERM(pTre2L);
    subdiv->rgn_start = pTre2L->rgn_offset[0] | pTre2L->rgn_offset[1] << 8 |
                        pTre2L->rgn_offset[2] << 16;

    subdiv_prev->rgn_end = subdiv->rgn_start;

    subdiv->hasPoints = pTre2L->elements & 0x10;
    subdiv->hasIdxPoints = pTre2L->elements & 0x20;
    subdiv->hasPolylines = pTre2L->elements & 0x40;
    subdiv->hasPolygons = pTre2L->elements & 0x80;

    subdiv->level = subdiv->maplevel->zoom;
    subdiv->shift = 24 - subdiv->maplevel->bits;

    cx = pTre2L->center_lng[0] | pTre2L->center_lng[1] << 8 |
         pTre2L->center_lng[2] << 16;
    subdiv->iCenterLng = cx;
    cy = pTre2L->center_lat[0] | pTre2L->center_lat[1] << 8 |
         pTre2L->center_lat[2] << 16;
    subdiv->iCenterLat = cy;
    width = TRE_SUBDIV_WIDTH(pTre2L) << subdiv->shift;
    height = pTre2L->height << subdiv->shift;

    subdiv->north = GARMIN_RAD(cy + height + 1);
    subdiv->south = GARMIN_RAD(cy - height);
    subdiv->east = GARMIN_RAD(cx + width + 1);
    subdiv->west = GARMIN_RAD(cx - width);

    subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north),
                          QPointF(subdiv->east, subdiv->south));

    subdiv_prev = subdiv;
    ++pTre2L;
    ++subdiv;
  }
}

void CGarminDecoder::readSubdivInfoExt(QFile& file, subfile_t& subfile) {
  // read extended elements' offsets and sizes
  file.seek(subfile.parts["GMP"].offset + subfile.hdrTRE.tre7_offset);
  // read offsets of 1st subdiv
  if (subfile.hdrTRE.tre7_rec_size >= 4) {
    file.read((char*)&subfile.subdivs[0].offsetPolygons2,
              sizeof(subdiv_t::offsetPolygons2));
  }

  if (subfile.hdrTRE.tre7_rec_size >= 8) {
    file.read((char*)&subfile.subdivs[0].offsetPolylines2,
              sizeof(subdiv_t::offsetPolylines2));
  }
  if (subfile.hdrTRE.tre7_rec_size >= 12) {
    file.read((char*)&subfile.subdivs[0].offsetPoints2,
              sizeof(subdiv_t::offsetPoints2));
  }

  const quint32 nSubdivs = subfile.subdivs.size();
  for (quint32 i = 1; i < nSubdivs; i++) {
    subdiv_t& subdiv = subfile.subdivs[i];
    subdiv_t& last = subfile.subdivs[i - 1];

    // read offset of all other subdivs and calculate size of previous ones
    if (subfile.hdrTRE.tre7_rec_size >= 4) {
      file.read((char*)&subdiv.offsetPolygons2,
                sizeof(subdiv_t::offsetPolygons2));
      last.lengthPolygons2 = subdiv.offsetPolygons2 - last.offsetPolygons2;
    }

    if (subfile.hdrTRE.tre7_rec_size >= 8) {
      file.read((char*)&subdiv.offsetPolylines2,
                sizeof(subdiv_t::offsetPolylines2));
      last.lengthPolylines2 = subdiv.offsetPolylines2 - last.offsetPolylines2;
    }
    if (subfile.hdrTRE.tre7_rec_size >= 12) {
      file.read((char*)&subdiv.offsetPoints2, sizeof(subdiv_t::offsetPoints2));
      last.lengthPoints2 = subdiv.offsetPoints2 - last.offsetPoints2;
    }
  }

  // calculate size of previous ones
  subfile.subdivs.last().lengthPolygons2 =
      subfile.hdrRGN.length_polyg2 - subfile.subdivs.last().offsetPolygons2;
  subfile.subdivs.last().lengthPolylines2 =
      subfile.hdrRGN.length_polyl2 - subfile.subdivs.last().offsetPolylines2;
  subfile.subdivs.last().lengthPoints2 =
      subfile.hdrRGN.length_point2 - subfile.subdivs.last().offsetPoints2;
}

void CGarminDecoder::readTre8(QFile& file, subfile_t& subfile) {
  print("--- overview ---\n");
  // read extended elements' offsets and sizes
  file.seek(subfile.parts["GMP"].offset + subfile.hdrTRE.tre8_offset);
  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::LittleEndian);

  const int N = subfile.hdrTRE.tre8_size / subfile.hdrTRE.tre8_rec_size;
  for (int n = 0; n < N; n++) {
    quint8 type, maxlevel, subtype;
    stream >> type >> maxlevel >> subtype;
    print("type: %02X subtype: %02X maxlevel: %i %i\n", type, subtype, maxlevel,
          (0x10000 | (quint32(type) << 8) | (quint32(subtype))));
  }
}

void CGarminDecoder::readStringTable(QFile& file, subfile_t& subfile) {
  quint32 offsetLbl1 = subfile.parts["GMP"].offset + subfile.hdrLBL.lbl1_offset;
  quint32 offsetLbl6 = subfile.parts["GMP"].offset + subfile.hdrLBL.lbl6_offset;

  quint16 codepage = 0;
  if (subfile.hdrLBL.size > 0xAA) {
    codepage = subfile.hdrLBL.codepage;
  }

  //         qDebug() << file.fileName() << Qt::hex << offsetLbl1 << offsetLbl6
  //         << offsetNet1;

  switch (subfile.hdrLBL.coding) {
    case 0x06:
      subfile.strtbl = new CGarminStrTbl6(codepage, 0, this);
      break;

    case 0x09:
      subfile.strtbl = new CGarminStrTbl8(codepage, 0, this);
      break;

    case 0x0A:
      subfile.strtbl = new CGarminStrTblUtf8(codepage, 0, this);
      break;

    case 0x0B:
      subfile.strtbl = new CGarminStrTblUtf8(codepage, 0, this);
      break;

    default:
      qWarning() << "Unknown label coding" << Qt::hex << subfile.hdrLBL.coding;
  }

  if (nullptr != subfile.strtbl) {
    subfile.strtbl->registerLBL1(offsetLbl1, subfile.hdrLBL.lbl1_length,
                                 subfile.hdrLBL.addr_shift);
    subfile.strtbl->registerLBL6(offsetLbl6, subfile.hdrLBL.lbl6_length);
  }
}

void CGarminDecoder::analyse() {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw CException("failed to open file: " + filename);
  }

  for (const subfile_t& subfile : subfiles) {
    if (subfile.subdivs.isEmpty()) {
      continue;
    }

    if (!subfile.hdrLBL.lbl29_length || !subfile.hdrLBL.lbl28_length) {
      continue;
    }

    for (const subdiv_t& subdiv : subfile.subdivs) {
      if (subdiv.maplevel->bits != 24) {
        continue;
      }

      print("--- subdiv start---\n");
      subdiv.print();
      fflush(stdout);
      fflush(stderr);

      file.seek(subfile.parts["GMP"].offset + subfile.hdrRGN.offset_polyg2);
      QByteArray dataRgnPolyg2 = file.read(subfile.hdrRGN.length_polyg2);

      quint8* pData = (quint8*)dataRgnPolyg2.data() + subdiv.offsetPolygons2;
      quint8* pEnd = pData + subdiv.lengthPolygons2;

      while (pData < pEnd) {
        if (*pData == 0xE0) {
          CGarminRaster tileEntry;
          pData += tileEntry.decode(pData, pEnd);

          qDebug("--------   tile  --------");
          qDebug() << tileEntry.index << tileEntry.lon1 << tileEntry.lat1
                   << tileEntry.lon2 << tileEntry.lat2 << tileEntry.size;
        } else {
          CGarminPolygon polygonEntry;
          int bytes = polygonEntry.decode2(subdiv.iCenterLng, subdiv.iCenterLat,
                                           subdiv.shift, false, pData, pEnd);

          if (subfile.strtbl) {
            subfile.strtbl->get(file, polygonEntry.lbl_info,
                                IGarminStrTbl::norm, polygonEntry.labels);
          }

          qDebug("\n-------- polygon --------");
          qDebug() << "lbl_in_NET" << polygonEntry.lbl_in_NET << "hasV2Label"
                   << polygonEntry.hasV2Label << "lbl_info"
                   << polygonEntry.lbl_info << polygonEntry.labels;
          qDebug() << polygonEntry.type << polygonEntry.points;

          QByteArray chunk((const char*)pData, bytes);
          for (quint8 byte : chunk) {
            print("%02X ", byte);
          }
          print("\n");
          pData += bytes;
        }
      }
      fflush(stdout);
      fflush(stderr);
      print("--- subdiv done---\n\n");
      fflush(stdout);
      fflush(stderr);
    }
  }
}

void CGarminDecoder::dumpSubfiles() {
  int numThreads = allArgs.value("j").toInt();
  if (numThreads > 1) {
    QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
  }

  for (const subfile_t& subfile : subfiles) {
    if (subfile.subdivs.isEmpty()) {
      continue;
    }

    if (!subfile.hdrLBL.lbl29_length || !subfile.hdrLBL.lbl28_length) {
      continue;
    }

    if (numThreads == 1) {
      processSubfile(subfile);
    } else {
      CSubfileTask* task =
          new CSubfileTask([this, &subfile]() { processSubfile(subfile); });

      QThreadPool::globalInstance()->start(task);
    }
  }

  QThreadPool::globalInstance()->waitForDone();

  for (const QPointF& scale : outputPaths.keys()) {
    qDebug() << scale;
  }
}

void CGarminDecoder::processSubfile(const subfile_t& subfile) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw CException("failed to open file: " + filename);
  }

  int nTiles = subfile.hdrLBL.lbl28_length / subfile.hdrLBL.lbl28_rec_size;

  printf("Process %s: number of tiles %i in LBL29\n",
         subfile.name.toUtf8().data(), nTiles);

  QVector<tile_t> tiles(nTiles);
  file.seek(subfile.parts["GMP"].offset + subfile.hdrRGN.offset_polyg2);
  QByteArray dataRgnPolyg2 = file.read(subfile.hdrRGN.length_polyg2);
  file.seek(subfile.parts["GMP"].offset + subfile.hdrLBL.lbl28_offset);
  QByteArray dataLbl28 = file.read(subfile.hdrLBL.lbl28_length);
  quint32* jpgOffsets = (quint32*)dataLbl28.data();

  print(">>> %08X %08X %08X\n", subfile.parts["GMP"].offset,
        subfile.hdrLBL.lbl28_offset,
        subfile.parts["GMP"].offset + subfile.hdrLBL.lbl28_offset);

  quint8* pData = (quint8*)dataRgnPolyg2.data();
  quint8* pEnd = pData + dataRgnPolyg2.size();

  CFuzzyMap<QList<quint32> > tilesPerLevel;

  QSet<quint32> tilesSeen;

  bool allowEmpty = allArgs.isSet("allow-empty");

  while (pData < pEnd) {
    if (*pData == 0xE0) {
      CGarminRaster tileEntry;
      pData += tileEntry.decode(pData, pEnd);

      if (tilesSeen.contains(tileEntry.index)) {
        continue;
      }
      tilesSeen.insert(tileEntry.index);

      // Load the tile and all it's information
      tile_t& tile = tiles[tileEntry.index];
      quint32 jpgOffset = jpgOffsets[tileEntry.index];
      tile.offset =
          subfile.parts["GMP"].offset + subfile.hdrLBL.lbl29_offset + jpgOffset;
      tile.lat1 = tileEntry.lat1;
      tile.lon1 = tileEntry.lon1;
      tile.lat2 = tileEntry.lat2;
      tile.lon2 = tileEntry.lon2;

      //            print("%08X %08X %08X ", subfile.parts["GMP"].offset,
      //            subfile.hdrLBL.lbl29_offset, jpgOffset); print("Tile#: %i
      //            Offset: %08X %i\n", tileEntry.index, tile.offset,
      //            tile.offset);

      file.seek(tile.offset);
      if (!tile.img.load(&file, "JPEG")) {
        throw CException("failed to load tile");
      }

      if (!allowEmpty && tile.isSingleColor()) {
        continue;
      }

      tile.w = tile.img.width();
      tile.h = tile.img.height();

      tile.scaleX = (tile.lon2 - tile.lon1) / tile.w;
      tile.scaleY = (tile.lat2 - tile.lat1) / tile.h;

      const QPointF scales(tile.scaleX, tile.scaleY);
      {
        QMutexLocker lock(&mutex);
        QString& outputPath = outputPaths[QPointF(scales)];
        if (outputPath.isEmpty()) {
          outputPath = QString("%1_%2").arg(basename).arg(outputPaths.size());
          QDir("./").mkpath(outputPath);
        }
      }

      tilesPerLevel[scales].append(tileEntry.index);
    } else {
      CGarminPolygon polygonEntry;
      pData += polygonEntry.decode2(0, 0, 0, false, pData, pEnd);
    }
  }

  if (allArgs.isSet("dry-run")) {
    return;
  }

  OGRSpatialReference oSRS;
  oSRS.importFromProj4("+proj=longlat");

  for (const QPointF& scales : tilesPerLevel.keys()) {
    double lon1 = 180.0;
    double lat1 = -90.0;
    double lon2 = -180.0;
    double lat2 = 90.0;

    const QList<quint32>& idxTiles = tilesPerLevel[scales];
    for (quint32 idxTile : idxTiles) {
      const tile_t& tile = tiles[idxTile];
      lon1 = qMin(tile.lon1, lon1);
      lat1 = qMax(tile.lat1, lat1);
      lon2 = qMax(tile.lon2, lon2);
      lat2 = qMin(tile.lat2, lat2);
    }

    int width = qRound((lon2 - lon1) / scales.x());
    int height = qRound((lat2 - lat1) / scales.y());

    GDALDataset* dataset = nullptr;
    {
      QMutexLocker lock(&mutex);

      QDir dir(outputPaths[scales]);
      QString outfile = dir.filePath(subfile.name + ".tif");
      dataset = driver->Create(outfile.toUtf8().data(), width, height, 4,
                               GDT_Byte, (char**)gtiffArgs);
    }

    dataset->GetRasterBand(4)->Fill(0);
    dataset->SetSpatialRef(&oSRS);

    double adfGeoTransform[6] = {0};
    adfGeoTransform[0] = lon1;
    adfGeoTransform[1] = scales.x();
    adfGeoTransform[2] = 0;
    adfGeoTransform[3] = lat1;
    adfGeoTransform[4] = 0;
    adfGeoTransform[5] = scales.y();
    dataset->SetGeoTransform(adfGeoTransform);

    for (quint32 idxTile : idxTiles) {
      tile_t& tile = tiles[idxTile];

      int w = qCeil((tile.lon2 - tile.lon1) / scales.x());
      int h = qCeil((tile.lat2 - tile.lat1) / scales.y());

      int offX = qMax(0, qFloor((tile.lon1 - lon1) / scales.x()));
      int offY = qMax(0, qFloor((tile.lat1 - lat1) / scales.y()));

      if ((offX + w) > width) {
        w = width - offX;
      }
      if ((offY + h) > height) {
        h = height - offY;
      }

      QImage img = tile.img.scaled(w, h, Qt::IgnoreAspectRatio,
                                   Qt::SmoothTransformation);

      GDALRasterBand* band;
      QByteArray alpha(w * h, 255);
      band = dataset->GetRasterBand(4);
      if (band->RasterIO(GF_Write, offX, offY, w, h, alpha.data(), w, h,
                         GDT_Byte, 1, 0) != CE_None) {
        qFatal("unable to write data");
      }
      band = dataset->GetRasterBand(3);
      if (band->RasterIO(GF_Write, offX, offY, w, h, img.bits() + 0, w, h,
                         GDT_Byte, 4, 0) != CE_None) {
        qFatal("unable to write data");
      }
      band = dataset->GetRasterBand(2);
      if (band->RasterIO(GF_Write, offX, offY, w, h, img.bits() + 1, w, h,
                         GDT_Byte, 4, 0) != CE_None) {
        qFatal("unable to write data");
      }
      band = dataset->GetRasterBand(1);
      if (band->RasterIO(GF_Write, offX, offY, w, h, img.bits() + 2, w, h,
                         GDT_Byte, 4, 0) != CE_None) {
        qFatal("unable to write data");
      }
    }

    dataset->FlushCache();
    delete dataset;
  }
}
