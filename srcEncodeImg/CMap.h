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
#ifndef CMAP_H
#define CMAP_H

#include <proj_api.h>
#include <QObject>


class QFile;
class CSubMap;
class GDALDataset;


class CMap : public QObject
{
public:
    CMap();
    virtual ~CMap();

    void setName(const QString& n);
    void setCopyright(const QString& c);
    const QString& getName(){return name;}

    void addMapSource(const QString& filename);

    void split();

    void writeMps(QDataStream& mps);

    /// input file information
    struct file_t
    {
        file_t() : dataset(0), pj(0){memset(colortable, 0, sizeof(colortable));}
        ~file_t(){}

        bool operator<(const file_t& other) const
        {
            return xscale < other.xscale;
        }


        QString filename;
        QString projection;
        GDALDataset * dataset;
        projPJ pj;

        double lon1;
        double lat1;
        double lon2;
        double lat2;

        double xref1;
        double yref1;
        double xref2;
        double yref2;

        quint32 width;
        quint32 height;

        double xscale;
        double yscale;

        double scale;

        quint32 colortable[256];
    };


    struct subfile_t
    {
        QString name;

        double lon1;
        double lat1;
        double lon2;
        double lat2;

        double xref1;
        double yref1;
        double xref2;
        double yref2;

        quint32 width;
        quint32 height;


        const file_t * file;
    };

    /// map level information
    struct level_t
    {
        level_t() : lon1(-90.0), lat1(180.0), lon2(90.0), lat2(-180.0), xscale(0), yscale(0), scale(0), next(0){}

        QList<const file_t*> files;

        double lon1;
        double lat1;
        double lon2;
        double lat2;

        double xref1;
        double yref1;
        double xref2;
        double yref2;

        quint32 width;
        quint32 height;

        double xscale;
        double yscale;

        double scale;
        int bits;

        level_t * next;
    };


    const QList<subfile_t>& getSubfiles(){return subfiles;}

private:
    double distance(const double u1, const double v1, const double u2, const double v2);
    void prinfFileinfo(const file_t& file);
    void split(level_t& level);

    QString name;
    QString copyright;

    projPJ wgs84;

    double lon1;
    double lat1;
    double lon2;
    double lat2;


    QList<file_t>    srcfiles;
    QList<level_t>   levels;
    QList<subfile_t> subfiles;

    struct scale_t
    {
        double scale;
        int bits;
    };

    static const scale_t scales[];
    static const int SIZE_SCALES;
};

#endif //CMAP_H

