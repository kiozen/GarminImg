/**********************************************************************************************
    Copyright (C) 2013 Oliver Eichler oliver.eichler@gmx.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <proj_api.h>
#include <QImage>
#include <QtCore>

#include "CGarminPolygon.h"
#include "CGarminRaster.h"
#include "Garmin.h"

static const char * gtiffArgs[] = {
    "BLOCKXSIZE=256"
    , "BLOCKYSIZE=256"
    , "TILED=YES"
    , "COMPRESS=JPEG"
    , NULL
};


static const double level2scale[] =
{
    2.0     // 0
    , 2.0    // 1
    , 2.0    // 2
    , 2.0    // 3
    , 2.0    // 4
    , 2.0    // 5
    , 2.0    // 6
    , 2.0    // 7
    , 2.0    // 8
    , 2.0    // 9
    , 2.0    // 10
    , 2.0    // 11
    , 2.0    // 12
    , 2.0    // 13
    , 2.0    // 14
    , 2.0    // 15
    , 2.0    // 16
    , 10.0   // 17
    , 8.0    // 18
    , 8.0    // 19
    , 6.0    // 20
    , 6.0    // 21
    , 4.0    // 22
    , 2.0    // 23
    , 2.0    // 24
};

static void minno(hdr_tre_t* trehdr, QByteArray& data)
{
    if(trehdr->flag & 0x80)
    {
        quint32 nlevels     = trehdr->tre1_size / sizeof(tre_1_t);

        quint8 key[5];
        quint8 tbl[trehdr->tre1_size];
        memcpy(tbl, data.data(), trehdr->tre1_size);

        key[0] = (((tbl[0] >> 4) +  8) & 0x0F);
        key[1] = (((tbl[3] >> 4) + 16) & 0x0F);
        key[2] = (((tbl[3] & 0x0F) + 16) & 0x0F);
        key[3] = ((tbl[4] >> 4) & 0x7);
        if(nlevels > 2)
        {
            key[3] ^= (((tbl[9] >> 4) + 16 - key[3]) & 0x08);
        }
        key[4] = (((tbl[2] >> 4) + 16 - 0) & 15);

        for (quint32 i = 0; i < nlevels * 4; i++)
        {
            tbl[i] = (
                ((((tbl[i] >> 4) + 16 - key[(i * 2)   % 5]) & 15) << 4) +
                ((((tbl[i] & 15) + 16 - key[(i * 2 + 1) % 5]) & 15)));
        }

        tre_1_t * ml = (tre_1_t *)tbl;
        for (quint32 i = 0; i < nlevels; i++)
        {
            ++ml;
        }

        memcpy(data.data(), tbl, trehdr->tre1_size);
        trehdr->flag &= ~0x80;
    }
}

int main(int argc, char ** argv)
{
    char tmpstr[64] = {0};
    QMap<QString, subfile_t> subfiles;

    QString filename = argv[1];
    QString basename = QFileInfo(filename).baseName();

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "failed to open file" << filename;
        exit(-1);
    }

    GDALAllRegister();
    GDALDriverManager * drvman  = GetGDALDriverManager();
    GDALDriver * driver         = drvman->GetDriverByName("GTiff");

    projPJ pjsrc = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");
    projPJ pjtar = pj_init_plus("+proj=merc +ellps=WGS84 +datum=WGS84 +no_defs");

    gmapsupp_imghdr_t imghdr;
    file.read((char*)&imghdr, sizeof(gmapsupp_imghdr_t));

    printf("---- Header --- size(%08X)\n", sizeof(gmapsupp_imghdr_t));
    imghdr.print();

    size_t blocksize = imghdr.blocksize();


    printf("---- FAT --- \n");
    FATBlock_t FATBlock;
    file.read((char*)&FATBlock, sizeof(FATBlock_t));
    while(FATBlock.flag)
    {
        if(file.atEnd())
        {
            printf("premature end of file\n");
            exit(-1);
        }

        memcpy(tmpstr, FATBlock.name, sizeof(FATBlock.name) + sizeof(FATBlock.type));
        tmpstr[sizeof(FATBlock.name) + sizeof(FATBlock.type)] = 0;

        if(FATBlock.size != 0 && !subfiles.contains(tmpstr) && tmpstr[0] != 0x20)
        {
            subfile_t& subfile = subfiles[tmpstr];
            subfile.name = tmpstr;

            memcpy(tmpstr, FATBlock.type, sizeof(FATBlock.type));
            tmpstr[sizeof(FATBlock.type)] = 0;

            subfile_part_t& part = subfile.parts[tmpstr];
            part.size   = FATBlock.size;
            part.offset = FATBlock.blocks[0] * blocksize;

            printf("%s %s %08X %08X\n", subfile.name.toLatin1().data(), tmpstr, part.offset, part.size);
        }

        file.read((char*)&FATBlock, sizeof(FATBlock_t));
    }

    foreach(const QString &key, subfiles.keys())
    {
        if(key.mid(8) != "GMP")
        {
            continue;
        }

        hdr_tre_t hdrTRE;
        hdr_rgn_t hdrRGN;
        hdr_lbl_t hdrLBL;
        hdr_nod_t hdrNOD;
        hdr_net_t hdrNET;
        hdr_dem_t hdrDEM;
        QVector<maplevel_t> mapLevels;
        QVector<subdiv_t>   subdivs;


        subfile_t& subfile = subfiles[key];
        file.seek(subfile.parts["GMP"].offset);

        printf("--- GMP Header %s (%08X)---\n", key.toLatin1().data(), (quint32)file.pos());

        gmp_hdr_t hdr;
        file.read((char*)&hdr, sizeof(hdr));
//        hdr.print();
        QString copyright(file.readLine());

        if(hdr.offsetTRE)
        {
            printf("   --- TRE header ---\n");
            file.seek(subfile.parts["GMP"].offset + hdr.offsetTRE);
            file.read((char*)&hdrTRE, sizeof(hdrTRE));
//            hdrTRE.print(subfile.parts["GMP"].offset);
        }
        if(hdr.offsetRGN)
        {
            printf("   --- RGN header ---\n");
            file.seek(subfile.parts["GMP"].offset + hdr.offsetRGN);
            file.read((char*)&hdrRGN, sizeof(hdrRGN));
//            hdrRGN.print(subfile.parts["GMP"].offset);
        }
        if(hdr.offsetLBL)
        {
            printf("   --- LBL header ---\n");
            file.seek(subfile.parts["GMP"].offset + hdr.offsetLBL);
            file.read((char*)&hdrLBL, sizeof(hdrLBL));
//            hdrLBL.print(subfile.parts["GMP"].offset);
        }
        if(hdr.offsetNOD)
        {
            printf("   --- NOD header ---\n");
            file.seek(subfile.parts["GMP"].offset + hdr.offsetNOD);
            file.read((char*)&hdrNOD, sizeof(hdrNOD));
//            hdrNOD.print();
        }
        if(hdr.offsetNET)
        {
            printf("   --- NET header ---\n");
            file.seek(subfile.parts["GMP"].offset + hdr.offsetNET);
            file.read((char*)&hdrNET, sizeof(hdrNET));
//            hdrNET.print();
        }
        if(hdr.offsetDEM)
        {
            printf("   --- DEM header ---\n");
            file.seek(subfile.parts["GMP"].offset + hdr.offsetDEM);
            file.read((char*)&hdrDEM, sizeof(hdrDEM));
//            hdrDEM.print(subfile.parts["GMP"].offset);
        }

        printf("   --- Analysis of map data ---\n");
        quint32 nLevels             = hdrTRE.tre1_size / sizeof(tre_1_t);
        quint32 nSubdivs            = 0;
        quint32 nSubdivsLast        = 0;
        int mapLevelIdx             = 0;

        // read map levels from TRE1
        file.seek(subfile.parts["GMP"].offset + hdrTRE.tre1_offset);
        QByteArray bufMapLevels = file.read(hdrTRE.tre1_size);

        if(hdrTRE.flag & 0x80)
        {
            minno(&hdrTRE, bufMapLevels);
        }

        tre_1_t * pMapLevel = (tre_1_t*)bufMapLevels.data();
        for(int i = 0; i < nLevels; i++)
        {
            nSubdivs    += pMapLevel->subdiv;
            nSubdivsLast = pMapLevel->subdiv;

            maplevel_t ml;
            ml.inherit  = pMapLevel->zoom & 0x80;
            ml.zoom     = pMapLevel->zoom & 0x7F;
            ml.bits     = pMapLevel->bits;
            ml.subdiv   = pMapLevel->subdiv;

            mapLevels << ml;
            pMapLevel->print();
            pMapLevel++;
        }

        quint32 nSubdivsNext = nSubdivs - nSubdivsLast;

        // resize number of sub-divisions
        subdivs.resize(nSubdivs);
        // #################################################
        // #### read subdiv information                  ###
        file.seek(subfile.parts["GMP"].offset + hdrTRE.tre2_offset);
        QByteArray tre2 = file.read(hdrTRE.tre2_size);

        tre_2_next_t * pTre2N = (tre_2_next_t*)tre2.data();

        QVector<subdiv_t>::iterator subdiv      = subdivs.begin();
        QVector<subdiv_t>::iterator subdiv_prev = subdivs.end();


        quint32 nSubdiv = mapLevels[mapLevelIdx].subdiv;
        // parse all 16 byte subdivision entries
        int l;
        for(l = 0; l < nSubdivsNext; ++l, --nSubdiv)
        {
            qint32 cx, cy;
            qint32 width, height;

            subdiv->maplevel     = &mapLevels[mapLevelIdx];
            subdiv->n            = l;
            subdiv->next         = pTre2N->next;
            subdiv->terminate    = TRE_SUBDIV_TERM(pTre2N);
            subdiv->rgn_start    = pTre2N->rgn_offset[0] | pTre2N->rgn_offset[1] << 8 | pTre2N->rgn_offset[2] << 16;

            // skip if this is the first entry
            if(subdiv_prev != subdivs.end())
            {
                subdiv_prev->rgn_end = subdiv->rgn_start;
            }

            subdiv->hasPoints    = pTre2N->elements & 0x10;
            subdiv->hasIdxPoints = pTre2N->elements & 0x20;
            subdiv->hasPolylines = pTre2N->elements & 0x40;
            subdiv->hasPolygons  = pTre2N->elements & 0x80;


            // if all subdivisions of this level have been parsed, switch to the next one
            if(nSubdiv == 0)
            {
                mapLevelIdx++;
                subdiv->maplevel = &mapLevels[mapLevelIdx];
                nSubdiv = subdiv->maplevel->subdiv;
            }

            subdiv->level = subdiv->maplevel->zoom;
            subdiv->shift = 24 - subdiv->maplevel->bits;

            cx = pTre2N->center_lng[0] | pTre2N->center_lng[1] << 8 | pTre2N->center_lng[2] << 16;
            subdiv->iCenterLng = cx;
            cy = pTre2N->center_lat[0] | pTre2N->center_lat[1] << 8 | pTre2N->center_lat[2] << 16;
            subdiv->iCenterLat = cy;
            width   = TRE_SUBDIV_WIDTH(pTre2N) << subdiv->shift;
            height  = pTre2N->height << subdiv->shift;

            subdiv->north = GARMIN_RAD(cy + height + 1);
            subdiv->south = GARMIN_RAD(cy - height);
            subdiv->east  = GARMIN_RAD(cx + width + 1);
            subdiv->west  = GARMIN_RAD(cx - width);

            subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north), QPointF(subdiv->east, subdiv->south));


            subdiv_prev = subdiv;
            ++pTre2N;
            ++subdiv;
        }

        mapLevelIdx++;
        tre_2_t* pTre2L = pTre2N;

        for(; l < nSubdivs; ++l)
        {
            qint32 cx, cy;
            qint32 width, height;
            subdiv->maplevel     = &mapLevels[mapLevelIdx];
            subdiv->n            = l;
            subdiv->next         = 0;
            subdiv->terminate    = TRE_SUBDIV_TERM(pTre2L);
            subdiv->rgn_start    = pTre2L->rgn_offset[0] | pTre2L->rgn_offset[1] << 8 | pTre2L->rgn_offset[2] << 16;
            subdiv_prev->rgn_end = subdiv->rgn_start;
            subdiv->hasPoints    = pTre2L->elements & 0x10;
            subdiv->hasIdxPoints = pTre2L->elements & 0x20;
            subdiv->hasPolylines = pTre2L->elements & 0x40;
            subdiv->hasPolygons  = pTre2L->elements & 0x80;

            subdiv->level = subdiv->maplevel->zoom;
            subdiv->shift = 24 - subdiv->maplevel->bits;

            cx = pTre2L->center_lng[0] | pTre2L->center_lng[1] << 8 | pTre2L->center_lng[2] << 16;
            subdiv->iCenterLng = cx;
            cy = pTre2L->center_lat[0] | pTre2L->center_lat[1] << 8 | pTre2L->center_lat[2] << 16;
            subdiv->iCenterLat = cy;
            width   = TRE_SUBDIV_WIDTH(pTre2L) << subdiv->shift;
            height  = pTre2L->height << subdiv->shift;

            subdiv->north = GARMIN_RAD(cy + height + 1);
            subdiv->south = GARMIN_RAD(cy - height);
            subdiv->east  = GARMIN_RAD(cx + width + 1);
            subdiv->west  = GARMIN_RAD(cx - width);

            subdiv->area = QRectF(QPointF(subdiv->west, subdiv->north), QPointF(subdiv->east, subdiv->south));

            subdiv_prev = subdiv;
            ++pTre2L;
            ++subdiv;
        }


        // #################################################
        // #### read extended elements' offsets and sizes###
        file.seek(subfile.parts["GMP"].offset + hdrTRE.tre7_offset);
        // read offsets of 1st subdiv
        file.read((char*)&subdivs[0].offsetPolygons2, sizeof(subdiv_t::offsetPolygons2));

        if(hdrTRE.tre7_rec_size >= 8)
        {
            file.read((char*)&subdivs[0].offsetPolylines2, sizeof(subdiv_t::offsetPolylines2));
        }
        if(hdrTRE.tre7_rec_size >= 12)
        {
            file.read((char*)&subdivs[0].offsetPoints2, sizeof(subdiv_t::offsetPoints2));
        }

        for(int i = 1; i < nSubdivs; i++)
        {
            subdiv_t& subdiv = subdivs[i];
            subdiv_t& last   = subdivs[i - 1];

            // read offset of all other subdivs and calculate size of previous ones
            file.read((char*)&subdiv.offsetPolygons2, sizeof(subdiv_t::offsetPolygons2));
            last.lengthPolygons2 = subdiv.offsetPolygons2 - last.offsetPolygons2;

            if(hdrTRE.tre7_rec_size >= 8)
            {
                file.read((char*)&subdiv.offsetPolylines2, sizeof(subdiv_t::offsetPolylines2));
                last.lengthPolylines2 = subdiv.offsetPolylines2 - last.offsetPolylines2;
            }
            if(hdrTRE.tre7_rec_size >= 12)
            {
                file.read((char*)&subdiv.offsetPoints2, sizeof(subdiv_t::offsetPoints2));
                last.lengthPoints2 = subdiv.offsetPoints2 - last.offsetPoints2;
            }
        }

        //calculate size of previous ones
        subdivs.last().lengthPolygons2  = hdrRGN.length_polyg2 - subdivs.last().offsetPolygons2;
        subdivs.last().lengthPolylines2 = hdrRGN.length_polyl2 - subdivs.last().offsetPolylines2;
        subdivs.last().lengthPoints2    = hdrRGN.length_point2 - subdivs.last().offsetPoints2;
        // ################################################

        // debug output
        printf("# of subdivs: %i\n", nSubdivs);

//        for(int i = 0; i < nSubdivs; i++)
//        {
//            subdiv_t& subdiv = subdivs[i];
//            subdiv.print();
//        }

        if(hdrLBL.lbl29_length && hdrLBL.lbl28_length)
        {
            printf("number of tiles %i in LBL29\n", hdrLBL.lbl28_length / hdrLBL.lbl28_rec_size);

            QImage img;
            CGarminPolygon p;
            CGarminRaster r;
            file.seek(subfile.parts["GMP"].offset + hdrRGN.offset_polyg2);
            QByteArray dataRgnPolyg2 = file.read(hdrRGN.length_polyg2);

            qDebug() << "lbl28" << hex << subfile.parts["GMP"].offset + hdrLBL.lbl28_offset;
            file.seek(subfile.parts["GMP"].offset + hdrLBL.lbl28_offset);
            QByteArray dataLbl28 = file.read(hdrLBL.lbl28_length);
            quint32 * jpgOffsets = (quint32*)dataLbl28.data();
            quint32 jpgOffset;

            QVector<tile_t> tiles(hdrLBL.lbl28_length / hdrLBL.lbl28_rec_size);

            for(int i = 0; i < nSubdivs; i++)
            {
                subdiv_t& subdiv = subdivs[i];
                QSet<int> tileIndexSeen;

                if(subdiv.lengthPolygons2 == 0)
                {
                    continue;
                }

                printf("subdiv #%i level %i", subdiv.n, subdiv.maplevel->zoom);
                quint8 * pData = (quint8*)dataRgnPolyg2.data() + subdiv.offsetPolygons2;
                quint8 * pEnd  = pData + subdiv.lengthPolygons2;

                while(pData < pEnd)
                {
                    if(*pData == 0xE0)
                    {
                        pData += r.decode(pData, pEnd);

                        if(tileIndexSeen.contains(r.index))
                        {
                            continue;
                        }
                        tileIndexSeen << r.index;

                        jpgOffset = jpgOffsets[r.index];

                        file.seek(subfile.parts["GMP"].offset + hdrLBL.lbl29_offset + jpgOffset);
                        if(!img.load(&file, "JPEG"))
                        {
                            qDebug() << "failed to load tile";
                            exit(-1);
                        }

                        int width = img.width();
                        int height = img.height();

                        double u1 = r.lon1 * DEG_TO_RAD;
                        double v1 = r.lat1 * DEG_TO_RAD;
                        double u2 = r.lon2 * DEG_TO_RAD;
                        double v2 = r.lat2 * DEG_TO_RAD;

                        pj_transform(pjsrc, pjtar, 1, 0, &u1, &v1, 0);
                        pj_transform(pjsrc, pjtar, 1, 0, &u2, &v2, 0);

                        tile_t& tile = tiles[r.index];
                        tile.offset = subfile.parts["GMP"].offset + hdrLBL.lbl29_offset + jpgOffset;
                        tile.lat1   = r.lat1;
                        tile.lon1   = r.lon1;
                        tile.lat2   = r.lat2;
                        tile.lon2   = r.lon2;
                        tile.u1     = u1;
                        tile.v1     = v1;
                        tile.u2     = u2;
                        tile.v2     = v2;
                        tile.w      = width;
                        tile.h      = height;
                        tile.bits << subdiv.maplevel->bits;


                        subdiv.maplevel->nTiles++;
                        subdiv.nTiles++;
                    }
                    else
                    {
                        pData += p.decode2(subdiv.iCenterLng, subdiv.iCenterLat, subdiv.shift, false, pData, pEnd);
                    }
                }

                printf(" nTiles #%i\n", subdiv.nTiles);
            }

            for(int i = 0; i < nLevels; i++)
            {
                maplevel_t& level = mapLevels[i];

//                level.scaleX = level.scaleX / level.nTiles;
//                level.scaleY = level.scaleY / level.nTiles;
                printf("bits %i level %i scale x: %f scale y: %f nTiles %i\n", level.bits, level.zoom, level.scaleX, level.scaleY, level.nTiles);

                bool noTiles = true;
                double lon1 =  180.0;
                double lat1 =  -90.0;
                double lon2 = -180.0;
                double lat2 =   90.0;
                double scaleX = 0.0;
                double scaleY = 0.0;
                foreach(const tile_t &tile, tiles)
                {
                    if(!tile.bits.contains(level.bits))
                    {
                        continue;
                    }

                    noTiles = false;

                    lon1 = qMin(tile.lon1, lon1);
                    lat1 = qMax(tile.lat1, lat1);
                    lon2 = qMax(tile.lon2, lon2);
                    lat2 = qMin(tile.lat2, lat2);
                }

                if(noTiles)
                {
                    continue;
                }

                lon1 *= DEG_TO_RAD;
                lat1 *= DEG_TO_RAD;
                lon2 *= DEG_TO_RAD;
                lat2 *= DEG_TO_RAD;

                pj_transform(pjsrc, pjtar, 1, 0, &lon1, &lat1, 0);
                pj_transform(pjsrc, pjtar, 1, 0, &lon2, &lat2, 0);

                scaleX =  level2scale[level.bits];
                scaleY = -level2scale[level.bits];

                int width  = round((lon2 - lon1) / scaleX);
                int height = round((lat2 - lat1) / scaleY);

                QString path = QString("%1_%2").arg(basename).arg(level.bits);
                QDir dir("./");
                dir.mkdir(path);
                dir.cd(path);

                QString outfile = dir.filePath(key + ".tif");
                GDALDataset * dataset = driver->Create(outfile.toLocal8Bit().data(), width, height, 3, GDT_Byte, (char**)gtiffArgs);

                OGRSpatialReference oSRS;
                oSRS.importFromProj4("+proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +no_defs");
                dataset->SetProjection((char*)&oSRS);

                double adfGeoTransform[6] = {0};
                adfGeoTransform[0] = lon1;
                adfGeoTransform[1] = scaleX;
                adfGeoTransform[2] = 0;
                adfGeoTransform[3] = lat1;
                adfGeoTransform[4] = 0;
                adfGeoTransform[5] = scaleY;
                dataset->SetGeoTransform(adfGeoTransform);

                GDALRasterBand * band;
                foreach(const tile_t &tile, tiles)
                {
                    if(!tile.bits.contains(level.bits))
                    {
                        continue;
                    }

                    int w = round((tile.u2 - tile.u1) / scaleX);
                    int h = round((tile.v2 - tile.v1) / scaleY);

                    file.seek(tile.offset);
                    img.load(&file, "JPEG");

                    int offX = qMax(0.0, round((tile.u1 - lon1) / scaleX));
                    int offY = qMax(0.0, round((tile.v1 - lat1) / scaleY));

                    if((offX + w) > width)
                    {
                        w = width  - offX;
                    }
                    if((offY + h) > height)
                    {
                        h = height - offY;
                    }

                    img = img.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

                    band = dataset->GetRasterBand(3);
                    if(band->RasterIO(GF_Write, offX, offY, w, h, img.bits() + 0, w, h, GDT_Byte, 4, 0) != CE_None)
                    {
                        qFatal("unable to write data");
                    }
                    band = dataset->GetRasterBand(2);
                    if(band->RasterIO(GF_Write, offX, offY, w, h, img.bits() + 1, w, h, GDT_Byte, 4, 0) != CE_None)
                    {
                        qFatal("unable to write data");
                    }
                    band = dataset->GetRasterBand(1);
                    if(band->RasterIO(GF_Write, offX, offY, w, h, img.bits() + 2, w, h, GDT_Byte, 4, 0) != CE_None)
                    {
                        qFatal("unable to write data");
                    }
                }

                dataset->FlushCache();
                delete dataset;
            }
        }
    }
    printf("\n\n\n");

    GDALDestroyDriverManager();
    return 0;
}
