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
#pragma once

#include "headers/CHdrTre.h"
#include "sections/CSection.h"
#include "sections/CTre1.h"
#include "sections/CTre2.h"
#include "sections/CTre3.h"
#include "sections/CTre7.h"
#include "sections/CTre8.h"


class CFileTre
{
public:
    CFileTre(const QString& mapNumber);
    virtual ~CFileTre() = default;

    const CHdrTre& header() const
    {
        return hdrTre;
    }

    void writeHdr(QFile& file);
    void writeData(QFile& file);

    void setTre7RecSize(quint16 size){tre7.setSizeRecord(size);}
    void addTre7Record(quint32 offsetPolygon2);
    void addTre7Record(quint32 offsetPolygon2, quint32 offsetPolyline2);
    void addTre7Record(quint32 offsetPolygon2, quint32 offsetPolyline2, quint32 offsetPoint2);

    void addTre8Polygon(quint32 type, quint8 maplevel);

    void finalizeSubdivs();

    void addCopyright(quint32 offset)
    {
        tre3.add(offset);
    }

    CSubdiv &addSubdiv(quint8 zoom, qreal northbound, qreal eastbound, qreal southbound, qreal westbound);

    void setMapNumber(const QString& str)
    {
        hdrTre.setMapId(str);
    }

    QList<CMapLevel>& maplevels()
    {
        return tre1.maplevels();
    }

    CSubdiv& subdivByNumber(CSubdiv::number_t number);



private:
    quint32 mapId = 0;

    CHdrTre hdrTre;
    CTre1 tre1;
    CTre2 tre2;
    CTre3 tre3;
    CSection tre4;
    CSection tre5;
    CSection tre6;
    CTre7 tre7;
    CTre8 tre8;
    CSection tre9;
    CSection tre10;

    qreal northbound_ = -90;
    qreal eastbound_ = -180;
    qreal southbound_ = 90;
    qreal westbound_ = 180;

    quint16 cntSubdiv_ = 0;
};


