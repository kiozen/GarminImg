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

#include "CGarminEncoder.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>

#include <QtCore>
#include <QtGui>

#include "CException.h"
#include "CFileGmp.h"
#include "CFileImg.h"
#include "CSubfileTask.h"

extern QCommandLineParser allArgs;
constexpr quint32 XSIZE_SUPER_TILE = 5120;
constexpr quint32 YSIZE_SUPER_TILE = 5120;
constexpr quint32 XSIZE_TILE = 512;
constexpr quint32 YSIZE_TILE = 512;

bool is_equal(const qreal lhs, const qreal rhs) { return fabs(lhs - rhs) <= std::numeric_limits<double>::epsilon(); }

CGarminEncoder::CGarminEncoder(const QString& filename)
    : filename(QDir::current().absoluteFilePath(filename)),
      basename(QFileInfo(filename).baseName()),
      tmpdir(allArgs.value("tmpdir") + "/" + basename + "_") {
  if (!tmpdir.isValid()) {
    throw CException("Could not create temp. working directory: " + tmpdir.path());
  }
  tmpdir.setAutoRemove(!allArgs.isSet("keep"));
}

void CGarminEncoder::print(const char* format, ...) {
  QMutexLocker lock(&mutex);
  va_list args;
  va_start(args, format);
  vfprintf(stdout, format, args);
  va_end(args);
}

bool CGarminEncoder::isSingleColor(const QImage& img) {
  quint32* p = (quint32*)img.bits();
  quint32 color = *p;
  const quint32 N = img.width() * img.height();
  for (quint32 n = 0; n < N; n++) {
    if (color != *p++) {
      return false;
    }
  }
  return true;
}

void CGarminEncoder::splitIntoTiles() {
  GDALDataset* dataset = (GDALDataset*)GDALOpen(filename.toUtf8(), GA_ReadOnly);
  if (nullptr == dataset) {
    throw CException("Failed to load raster map: " + filename);
  }

  // ------- setup projection ---------------
  const OGRSpatialReference* oSRS = dataset->GetSpatialRef();
  char* proj4 = nullptr;
  oSRS->exportToProj4(&proj4);
  CProj proj(proj4, "EPSG:4326");
  free(proj4);

  if (!proj.isValid()) {
    GDALClose(dataset);
    throw CException("No georeference information found. " + filename);
  }

  // ------- conversion matrix ---------------
  qreal adfGeoTransform[6];
  dataset->GetGeoTransform(adfGeoTransform);

  QTransform trFwd;
  trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
  trFwd.scale(adfGeoTransform[1], adfGeoTransform[5]);

  if (adfGeoTransform[4] != 0.0) {
    trFwd.rotate(qAtan(adfGeoTransform[2] / adfGeoTransform[4]));
  }

  // ------- bounds and sanity checks ---------------
  quint32 xsize_px = dataset->GetRasterXSize();
  quint32 ysize_px = dataset->GetRasterYSize();

  ref1 = trFwd.map(QPointF(0, 0));
  ref2 = trFwd.map(QPointF(xsize_px, 0));
  ref3 = trFwd.map(QPointF(xsize_px, ysize_px));
  ref4 = trFwd.map(QPointF(0, ysize_px));

  // projection sanity check. We support Mercator-like projections like:
  // "+proj=merc" or "+proj=longlat"
  QPointF _ref1 = ref1;
  QPointF _ref2 = ref2;
  QPointF _ref3 = ref3;
  QPointF _ref4 = ref4;

  if (proj.isSrcLatLong()) {
    _ref1 *= DEG_TO_RAD;
    _ref2 *= DEG_TO_RAD;
    _ref3 *= DEG_TO_RAD;
    _ref4 *= DEG_TO_RAD;
  }

  proj.transform(_ref1, PJ_FWD);
  proj.transform(_ref2, PJ_FWD);
  proj.transform(_ref3, PJ_FWD);
  proj.transform(_ref4, PJ_FWD);

  if (!is_equal(_ref1.y(), _ref2.y()) || !is_equal(_ref3.y(), _ref4.y()) || !is_equal(_ref1.x(), _ref4.x()) ||
      !is_equal(_ref2.x(), _ref3.x())) {
    GDALClose(dataset);
    throw CException(
        "Bad projection. Must be Mercator-like ('+proj=merc' or "
        "'+proj=longlat'). " +
        filename);
  }

  // ------- export tiles ---------------
  quint32 xoff = 0;
  quint32 yoff = 0;

  QString subFileBaseName = QString::number(qAbs(qFloor(_ref1.x() * RAD_TO_DEG))).rightJustified(2, '0') +
                            QString::number(qAbs(qFloor(_ref1.y() * RAD_TO_DEG))).rightJustified(2, '0');

  const int N = qCeil(qreal(xsize_px) / XSIZE_SUPER_TILE);
  const int M = qCeil(qreal(ysize_px) / YSIZE_SUPER_TILE);

  qint32 subfileCount = 0;

  int numThreads = allArgs.value("j").toInt();
  if (numThreads > 1) {
    QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
  }

  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      QString subfileName = subFileBaseName + QString::number(subfileCount++).rightJustified(4, '0');

      quint32 xsize = qMin(XSIZE_SUPER_TILE, xsize_px - xoff);
      quint32 ysize = qMin(YSIZE_SUPER_TILE, ysize_px - yoff);

      if (numThreads == 1) {
        exportSubfileTile(xoff, yoff, xsize, ysize, filename, subfileName);
      } else {
        CSubfileTask* task = new CSubfileTask([this, xoff, yoff, xsize, ysize, subfileName]() {
          exportSubfileTile(xoff, yoff, xsize, ysize, filename, subfileName);
        });
        QThreadPool::globalInstance()->start(task);
      }
      xoff += XSIZE_SUPER_TILE;
      // goto done1;
    }
    yoff += YSIZE_SUPER_TILE;
    xoff = 0;
  }
done1:

  if (numThreads != 1) {
    QThreadPool::globalInstance()->waitForDone();
  }

  GDALClose(dataset);
}

void CGarminEncoder::exportSubfileTile(quint32 xoff, quint32 yoff, quint32 xsize, quint32 ysize,
                                       const QString& filename, const QString& subfileName) {
  GDALDataset* dataset = nullptr;
  try {
    dataset = (GDALDataset*)GDALOpen(filename.toUtf8().data(), GA_ReadOnly);
    if (nullptr == dataset) {
      throw CException("Failed to load raster map: " + filename);
    }

    // ------- setup color table ---------
    QVector<QRgb> colortable;
    int rasterBandCount = dataset->GetRasterCount();
    if (rasterBandCount == 1) {
      GDALRasterBand* pBand = dataset->GetRasterBand(1);

      if (nullptr == pBand) {
        GDALClose(dataset);
        throw CException("No bands in file: " + filename);
      }

      if (pBand->GetColorInterpretation() == GCI_PaletteIndex) {
        GDALColorTable* pct = pBand->GetColorTable();
        for (int i = 0; i < pct->GetColorEntryCount(); ++i) {
          const GDALColorEntry& e = *pct->GetColorEntry(i);
          colortable << qRgba(e.c1, e.c2, e.c3, e.c4);
        }
      } else if (pBand->GetColorInterpretation() == GCI_GrayIndex) {
        for (int i = 0; i < 256; ++i) {
          colortable << qRgba(i, i, i, 255);
        }
      } else {
        GDALClose(dataset);
        throw CException("File must be 8 bit palette or gray indexed. " + filename);
      }

      int success = 0;
      qreal idx = pBand->GetNoDataValue(&success);
      if (success) {
        if ((idx >= 0) && (idx < colortable.size())) {
          QColor tmp(colortable[idx]);
          tmp.setAlpha(0);
          colortable[idx] = tmp.rgba();
        } else {
          throw CException("Index for no data value is out of bound. " + filename);
        }
      }
    }

    // ------- setup projection ---------------
    const OGRSpatialReference* oSRS = dataset->GetSpatialRef();
    char* proj4 = nullptr;
    oSRS->exportToProj4(&proj4);
    CProj proj(proj4, "EPSG:4326");
    free(proj4);

    if (!proj.isValid()) {
      GDALClose(dataset);
      throw CException("No georeference information found. " + filename);
    }

    // ------- conversion matrix ---------------
    qreal adfGeoTransform[6];
    dataset->GetGeoTransform(adfGeoTransform);

    QTransform trFwd;
    trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
    trFwd.scale(adfGeoTransform[1], adfGeoTransform[5]);

    if (adfGeoTransform[4] != 0.0) {
      trFwd.rotate(qAtan(adfGeoTransform[2] / adfGeoTransform[4]));
    }

    QDir dir;
    {
      QMutexLocker lock(&mutex);
      dir.setPath(tmpdir.path());
    }

    QPointF ref1 = trFwd.map(QPointF(xoff, yoff));
    QPointF ref2 = trFwd.map(QPointF(xoff + xsize, yoff + ysize));

    if (proj.isSrcLatLong()) {
      ref1 *= DEG_TO_RAD;
      ref2 *= DEG_TO_RAD;
    }

    proj.transform(ref1, PJ_FWD);
    proj.transform(ref2, PJ_FWD);

    const int N = qCeil(qreal(xsize) / XSIZE_TILE);
    const int M = qCeil(qreal(ysize) / YSIZE_TILE);

    quint32 xoffTile = 0;
    quint32 yoffTile = 0;

    CFileGmp gmpFile(subfileName);
    gmpFile.addCopyright("Copyright <someone>");
    CSubdiv& subdiv1 = gmpFile.addSubdiv(2, ref1.y() * RAD_TO_DEG, ref2.x() * RAD_TO_DEG, ref2.y() * RAD_TO_DEG,
                                         ref1.x() * RAD_TO_DEG);
    CSubdiv& subdiv2 = gmpFile.addSubdiv(1, ref1.y() * RAD_TO_DEG, ref2.x() * RAD_TO_DEG, ref2.y() * RAD_TO_DEG,
                                         ref1.x() * RAD_TO_DEG);
    CSubdiv& subdiv3 = gmpFile.addSubdiv(0, ref1.y() * RAD_TO_DEG, ref2.x() * RAD_TO_DEG, ref2.y() * RAD_TO_DEG,
                                         ref1.x() * RAD_TO_DEG);
    subdiv1.setNext(subdiv2.number());
    subdiv2.setNext(subdiv3.number());

    QList<CSubdiv::number_t> subdivNumbers;
    subdivNumbers << subdiv1.number();
    subdivNumbers << subdiv2.number();
    subdivNumbers << subdiv3.number();

    qint32 tileCount = 0;
    for (int m = 0; m < M; m++) {
      for (int n = 0; n < N; n++) {
        quint32 xsizeTile = qMin(XSIZE_TILE, xsize - xoffTile) & 0xFFFFFFFC;
        quint32 ysizeTile = qMin(YSIZE_TILE, ysize - yoffTile);
        quint32 x = xoff + xoffTile;
        quint32 y = yoff + yoffTile;

        if (rasterBandCount == 1) {
          GDALRasterBand* pBand;
          pBand = dataset->GetRasterBand(1);

          QImage img(QSize(xsizeTile, ysizeTile), QImage::Format_Indexed8);
          img.setColorTable(colortable);

          CPLErr err =
              pBand->RasterIO(GF_Read, x, y, xsizeTile, ysizeTile, img.bits(), xsizeTile, ysizeTile, GDT_Byte, 0, 0);
          if (err != CE_None) {
            throw CException("Failed to export tile from " + subfileName);
          }

          img = img.convertToFormat(QImage::Format_ARGB32);
          saveTile(img, trFwd, x, y, xsizeTile, ysizeTile, dir, proj, ++tileCount, gmpFile, subdivNumbers);

        } else {
          QImage img(QSize(xsizeTile, ysizeTile), QImage::Format_ARGB32);
          img.fill(qRgba(255, 255, 255, 255));

          QVector<quint8> buffer(xsizeTile * ysizeTile);

          QRgb testPix = qRgba(GCI_RedBand, GCI_GreenBand, GCI_BlueBand, GCI_AlphaBand);

          for (int b = 1; b <= rasterBandCount; ++b) {
            GDALRasterBand* pBand;
            pBand = dataset->GetRasterBand(b);

            int err = pBand->RasterIO(GF_Read, x, y, xsizeTile, ysizeTile, buffer.data(), xsizeTile, ysizeTile,
                                      GDT_Byte, 0, 0);

            if (!err) {
              int pbandColour = pBand->GetColorInterpretation();
              unsigned int offset;

              for (offset = 0; offset < sizeof(testPix) && *(((quint8*)&testPix) + offset) != pbandColour; offset++) {
              }
              if (offset < sizeof(testPix)) {
                quint8* pTar = img.bits() + offset;
                quint8* pSrc = buffer.data();
                const int size = buffer.size();

                for (int i = 0; i < size; ++i) {
                  *pTar = *pSrc;
                  pTar += sizeof(testPix);
                  pSrc += 1;
                }
              }
            } else {
              throw CException("Failed to export tile from " + subfileName);
            }
          }
          saveTile(img, trFwd, x, y, xsizeTile, ysizeTile, dir, proj, ++tileCount, gmpFile, subdivNumbers);
        }
        xoffTile += XSIZE_TILE;
        // goto done2;
      }
      yoffTile += XSIZE_TILE;
      xoffTile = 0;
    }
  done2:
    gmpFile.write(dir);

    if (tileCount == 0) {
      gmpFile.remove(dir);
    }

    print("Exported tiles for sub-file %s \n", subfileName.toUtf8().data());
  } catch (const CException& e) {
    qFatal("%s", e.what());
  }

  if (dataset) {
    GDALClose(dataset);
  }
}

void CGarminEncoder::saveTile(const QImage& img, const QTransform& trFwd, quint32 x, quint32 y, quint32 xsizeTile,
                              quint32 ysizeTile, const QDir& path, const CProj& proj, quint32 tileCount,
                              CFileGmp& gmpFile, const QList<CSubdiv::number_t>& subdivNumbers) {
  if (!isSingleColor(img)) {
    const QString& tilepath = path.absoluteFilePath(QString("%1_%2_%3.jpg").arg(tileCount).arg(ref1.y()).arg(ref1.x()));
    img.save(tilepath);

    QPointF ref1 = trFwd.map(QPointF(x, y));
    QPointF ref2 = trFwd.map(QPointF(x + xsizeTile, y + ysizeTile));

    if (proj.isSrcLatLong()) {
      ref1 *= DEG_TO_RAD;
      ref2 *= DEG_TO_RAD;
    }

    proj.transform(ref1, PJ_FWD);
    proj.transform(ref2, PJ_FWD);

    gmpFile.addTile(tilepath, ref1.y() * RAD_TO_DEG, ref2.x() * RAD_TO_DEG, ref2.y() * RAD_TO_DEG,
                    ref1.x() * RAD_TO_DEG, subdivNumbers);

    QFile::remove(tilepath);
  }
}

void CGarminEncoder::createImage(const QString& filename, const QString& description) {
  CFileImg tableFAT;
  tableFAT.setDescription(description);
  QDir dir(tmpdir.path());
  const QString& filenameMps = QString::number(tableFAT.hash(), 16).toUpper() + ".mps";

  QFile fileMps(dir.absoluteFilePath(filenameMps));
  if (!fileMps.open(QIODevice::WriteOnly)) {
    throw CException("Failed to open file " + fileMps.fileName());
  }
  QDataStream mps(&fileMps);
  mps.setByteOrder(QDataStream::LittleEndian);

  quint16 lengthDescription = description.toLatin1().size() + 1;
  quint16 idProduct = 1;
  quint16 idFamily = 2300;

  mps << quint8(0x46) << quint16(4 + lengthDescription) << idProduct << idFamily;
  mps.writeRawData(description.toLatin1(), lengthDescription);

  for (const QFileInfo& info : dir.entryInfoList(QDir::Files, QDir::Name)) {
    if (filenameMps == info.fileName()) {
      continue;
    }

    tableFAT << info;

    mps << quint8(0x4c) << quint16(8 + 2 * lengthDescription + 9);
    mps << idProduct << idFamily << quint32(0);
    mps.writeRawData(description.toLatin1(), lengthDescription);
    mps.writeRawData(description.toLatin1(), lengthDescription);
    mps << quint8(0);
    mps << quint32(info.baseName().toUInt()) << quint32(0);
  }

  fileMps.close();
  tableFAT << QFileInfo(fileMps.fileName());
  tableFAT.finalize();

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    throw CException("Failed to open file " + file.fileName());
  }

  tableFAT.write(file);

  file.close();
}
