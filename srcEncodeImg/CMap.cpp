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
#include "CMap.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QtCore>


const CMap::scale_t CMap::scales[] =
{
    {                            //0
        70000.0, 2
    }
    ,                            //1
    {
        50000.0, 3
    }
    ,                            //2
    {
        30000.0, 4
    }
    ,                            //3
    {
        20000.0, 5
    }
    ,                            //4
    {
        15000.0, 6
    }
    ,                            //5
    {
        10000.0, 7
    }
    ,                            //6
    {
        7000.0, 8
    }
    ,                            //7
    {
        5000.0, 9
    }
    ,                            //8
    {
        3000.0, 10
    }
    ,                            //9
    {
        2000.0, 11
    }
    ,                            //10
    {
        1500.0, 12
    }
    ,                            //11
    {
        1000.0, 13
    }
    ,                            //12
    {
        700.0, 14
    }
    ,                            //13
    {
        500.0, 15
    }
    ,                            //14
    {
        300.0, 16
    }
    ,                            //15
    {
        200.0, 17
    }
    ,                            //16
    {
        150.0, 18
    }
    ,                            //17
    {
        70.0, 19
    }
    ,                            //19
    {
        50.0, 20
    }
    ,                            //20
    {
        20.0, 21
    }
    ,                            //22
    {
        10.0, 22
    }
    ,                            //24
    {
        5.0, 23
    }
    ,                            //26
    {
        2.0, 24
    }
};

const int CMap::SIZE_SCALES = sizeof(CMap::scales) / sizeof(CMap::scale_t);

#define TILE_SIZE 512
#define N_GMP_TILES_X   10
#define N_GMP_TILES_Y   10
#define N_SUB_TILES_X   10
#define N_SUB_TILES_Y   10

CMap::CMap()
    : lon1( -90.0)
    , lon2(  90.0)
    , lat1( 180.0)
    , lat2(-180.0)
{
    wgs84 = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");
}

CMap::~CMap()
{
    pj_free(wgs84);
}

void CMap::setName(const QString& n)
{
    name = n;
}

void CMap::setCopyright(const QString& c)
{
    QFileInfo fi(c);

    if(fi.isFile())
    {
        QFile f(c);
        f.open(QIODevice::ReadOnly);
        copyright = f.readAll();
        f.close();
    }
    else
    {
        copyright = c;
    }
}

void CMap::addMapSource(const QString& filename)
{
    char projstr[1024];
    OGRSpatialReference oSRS;

    GDALDataset * dataset = (GDALDataset*)GDALOpen(filename.toLocal8Bit().data(), GA_ReadOnly);
    if(dataset == 0)
    {
        fprintf(stderr, "\nFailed to open %s\n", filename.toLocal8Bit().data());
        exit(-1);
    }

    projPJ pj;
    char * ptr = projstr;

    strncpy(projstr, dataset->GetProjectionRef(), sizeof(projstr));
    oSRS.importFromWkt(&ptr);
    oSRS.exportToProj4(&ptr);

    pj = pj_init_plus(ptr);
    if(pj == 0)
    {
        fprintf(stderr, "\nUnknown projection in file %s\n", filename.toLocal8Bit().data());
        exit(-1);
    }

    double adfGeoTransform[6];
    dataset->GetGeoTransform( adfGeoTransform );


    srcfiles.append(file_t());
    file_t& file = srcfiles.last();
    file.filename   = filename;
    file.projection = ptr;
    file.dataset    = dataset;
    file.pj         = pj;
    file.width      = dataset->GetRasterXSize();
    file.height     = dataset->GetRasterYSize();
    file.xscale     = adfGeoTransform[1];
    file.yscale     = adfGeoTransform[5];
    file.xref1      = adfGeoTransform[0];
    file.yref1      = adfGeoTransform[3];
    file.xref2      = file.xref1 + file.width  * file.xscale;
    file.yref2      = file.yref1 + file.height * file.yscale;

    if(pj_is_latlong(file.pj))
    {
        file.lon1 = file.xref1;
        file.lat1 = file.yref1;
        file.lon2 = file.xref2;
        file.lat2 = file.yref2;
    }
    else
    {
        file.lon1 = file.xref1;
        file.lat1 = file.yref1;
        file.lon2 = file.xref2;
        file.lat2 = file.yref2;

        pj_transform(pj, wgs84, 1, 0, &file.lon1, &file.lat1, 0);
        pj_transform(pj, wgs84, 1, 0, &file.lon2, &file.lat2, 0);

        file.lon1 *= RAD_TO_DEG;
        file.lat1 *= RAD_TO_DEG;
        file.lon2 *= RAD_TO_DEG;
        file.lat2 *= RAD_TO_DEG;
    }

    // fill color table if necessary
    GDALRasterBand * pBand;
    pBand = dataset->GetRasterBand(1);

    if(pBand->GetColorInterpretation() == GCI_PaletteIndex)
    {
        GDALColorTable * pct = pBand->GetColorTable();
        for(int c = 0; c < pct->GetColorEntryCount(); ++c)
        {
            const GDALColorEntry& e = *pct->GetColorEntry(c);
            file.colortable[c] = e.c1 | (e.c2 << 8) | (e.c3 << 16) | (e.c4 << 24);
        }
    }
    else if(pBand->GetColorInterpretation() ==  GCI_GrayIndex )
    {
        for(int c = 0; c < 256; ++c)
        {
            file.colortable[c] = c | (c << 8) | (c << 16) | 0xFF000000;
        }
    }

    int success = 0;
    int idx = (int)pBand->GetNoDataValue(&success);

    if(success)
    {
        file.colortable[idx] &= 0x00FFFFFF;
    }

    double dist = distance(file.lon1 * DEG_TO_RAD, file.lat1 * DEG_TO_RAD, file.lon2 * DEG_TO_RAD, file.lat1 * DEG_TO_RAD);
    file.scale = dist / file.width;

    if(file.lon1 > lon1)
    {
        lon1 = file.lon1;
    }
    if(file.lon2 < lon2)
    {
        lon2 = file.lon2;
    }
    if(file.lat1 < lat1)
    {
        lat1 = file.lat1;
    }
    if(file.lat2 > lat2)
    {
        lat2 = file.lat2;
    }
}

void CMap::split()
{
    double scale = 0.0;
    level_t * last = 0;

    qSort(srcfiles);
    foreach(const file_t &file, srcfiles)
    {
        prinfFileinfo(file);

        if(((fabs(scale - file.scale)) / scale) > 0.02)
        {
            levels.append(level_t());
            scale = file.scale;

            if(last != 0)
            {
                last->next  = &levels.last();
                last        = last->next;
            }
        }

        level_t& level = levels.last();

        level.scale  += scale;
        level.xscale += file.xscale;
        level.yscale += file.yscale;

        level.files.append(&file);

        if(file.lon1 > level.lon1)
        {
            level.lon1 = file.lon1;
        }
        if(file.lon2 < level.lon2)
        {
            level.lon2 = file.lon2;
        }
        if(file.lat1 < level.lat1)
        {
            level.lat1 = file.lat1;
        }
        if(file.lat2 > level.lat2)
        {
            level.lat2 = file.lat2;
        }
    }

    int cntSubfile = 1;
    for(int i = 0; i < levels.count(); i++)
    {
        level_t& l = levels[i];

        projPJ pj    = l.files.first()->pj;
        l.xscale    /= l.files.count();
        l.yscale    /= l.files.count();
        l.scale     /= l.files.count();

        l.xref1     = l.lon1 * DEG_TO_RAD;
        l.yref1     = l.lat1 * DEG_TO_RAD;
        l.xref2     = l.lon2 * DEG_TO_RAD;
        l.yref2     = l.lat2 * DEG_TO_RAD;

        pj_transform(wgs84, pj, 1, 0, &l.xref1, &l.yref1, 0);
        pj_transform(wgs84, pj, 1, 0, &l.xref2, &l.yref2, 0);

        l.width     = qRound((l.xref2 - l.xref1) / l.xscale);
        l.height    = qRound((l.yref2 - l.yref1) / l.yscale);

        for(int n = SIZE_SCALES - 1; n >= 0; n--)
        {
            if(scales[n].scale > l.scale)
            {
                l.bits = scales[n].bits;
                break;
            }
        }

        qDebug();
        qDebug() << "level" << i << "scale" << l.scale << "bits" << l.bits;
        qDebug() << "  width" << l.width << "height" << l.height;
        qDebug() << "  " << l.lon1 << l.lat1 << l.lon2 << l.lat2;

        foreach(const file_t * file, l.files)
        {
            subfiles.append(subfile_t());
            subfile_t& subfile = subfiles.last();

            subfile.name = QString("%1GMP").arg(cntSubfile++, 7, 10, QChar('0'));

            subfile.lat1    = file->lat1;
            subfile.lon1    = file->lon1;
            subfile.lat2    = file->lat2;
            subfile.lon2    = file->lon2;

            subfile.xref1   = file->xref1;
            subfile.yref1   = file->yref1;
            subfile.xref2   = file->xref2;
            subfile.yref2   = file->yref2;

            subfile.width   = file->width;
            subfile.height  = file->height;
        }
    }
}


void CMap::writeMps(QDataStream &mps)
{
}


double CMap::distance(const double u1, const double v1, const double u2, const double v2)
{
    double dU = u2 - u1; // lambda
    double dV = v2 - v1; // roh

    double d = 2 * asin(sqrt(sin(dV / 2) * sin(dV / 2) + cos(v1) * cos(v2) * sin(dU / 2) * sin(dU / 2)));

    return 6371010 * d;
}

void CMap::prinfFileinfo(const file_t& file)
{
    printf("\n\n----------------------");
    printf("\n%s:", file.filename.toLocal8Bit().data());
    printf("\nprojection: %s", file.projection.toLocal8Bit().data());
    printf("\nwidth: %i pixel height: %i pixel", file.width, file.height);

    if(pj_is_latlong(file.pj))
    {
        printf("\narea (top/left, bottom/right): %f %f, %f %f", file.lat1, file.lon1, file.lat2, file.lon2);
        printf("\nxscale: %f °/px, yscale: %f °/px", file.xscale, file.yscale);
    }
    else
    {
        printf("\narea (top/left, bottom/right): %f %f, %f %f", file.lat1, file.lon1, file.lat2, file.lon2);
        printf("\nxscale: %f m/px, yscale: %f m/px", file.xscale, file.yscale);
    }
    printf("\nreal scale: %f m/px\n", file.scale);
}
