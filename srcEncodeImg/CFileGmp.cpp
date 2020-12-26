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

#include "CException.h"
#include "CFileGmp.h"
#include "items/CTile.h"
#include "version.h"

#include <QtGui>

QStringList CFileGmp::copyright1 = {"This map has been crated by " WHAT_STR,
                                    "It might contain copyrighted data and might be resricted to distributon and copying."
                                    "Please make sure you have all rights necessary befor you use or copy this map.",
                                    WHAT_STR " is licensed unter GPL V3+. https://github.com/Maproom/qmapshack"};
QStringList CFileGmp::copyright2 = {"Please make sure you have all rights necessary befor you use or copy tis map"};


CFileGmp::CFileGmp(const QString &name)
    : name(name)
    , tre(name)
{
    for(const QString& copyright : copyright1)
    {
        copyrightData1.append(copyright.toLatin1());
        copyrightData1.append('\0');
    }

    for(const QString& copyright : copyright2)
    {
        copyrightData2.append(copyright.toLatin1());
        copyrightData2.append('\0');
    }

    lbl.addLabel("");
    lbl.addLabel("Raster Map");
    tre.addCopyright(lbl.addLabel("Created by " WHAT_STR  ". Software published under GPL V3+"));
    tre.setMapNumber(name);
}

void CFileGmp::addCopyright(const QString& str)
{
    tre.addCopyright(lbl.addLabel(str));
}

CSubdiv &CFileGmp::addSubdiv(quint8 maplevel, qreal northbound, qreal eastbound, qreal southbound, qreal westbound)
{
    return tre.addSubdiv(maplevel, northbound, eastbound, southbound, westbound);
}

void CFileGmp::addTile(const QString& path,
                       qreal northbound, qreal eastbound,
                       qreal southbound, qreal westbound,
                       QList<qint16> subdivNumbers)
{
    auto [indexTile, sizeTile, offsetLabel] = lbl.addTile(path);
    CTile tile(indexTile, sizeTile, offsetLabel);
    tile.setBoundaries(northbound, eastbound, southbound, westbound);

    for(qint16 number : subdivNumbers)
    {
        CSubdiv& subdiv = tre.subdivByNumber(number);
        subdiv.addTile(tile);
        tre.addTre8Polygon(tile.type(), subdiv.zoom());
    }
    hasPolygon2Section = true;
}

void CFileGmp::write(const QDir& path)
{
    QFile file(path.absoluteFilePath(name + ".gmp"));
    if(!file.open(QIODevice::WriteOnly))
    {
        throw CException("Failed to open file " + file.fileName());
    }

    // finalize some sections like tre2 & tre7 & rgn2..4
    quint16 tre7RecSize = 0;
    if(hasPoint2Section)
    {
        tre7RecSize = 12;
    }
    else if(hasPolyline2Section)
    {
        tre7RecSize = 8;
    }
    else if(hasPolygon2Section)
    {
        tre7RecSize = 4;
    }
    tre.setTre7RecSize(tre7RecSize);

    for(CMapLevel& maplevel : tre.maplevels())
    {
        for(CSubdiv& subdiv : maplevel.subdivs())
        {
            if(tre7RecSize == 12)
            {
                tre.addTre7Record(
                    rgn.addPolygons2(subdiv.polygon2()),
                    rgn.addPolylines2(subdiv.polyline2()),
                    rgn.addPoints2(subdiv.point2())
                    );
            }
            if(tre7RecSize > 4)
            {
                tre.addTre7Record(
                    rgn.addPolygons2(subdiv.polygon2()),
                    rgn.addPolylines2(subdiv.polyline2())
                    );
            }
            if(tre7RecSize > 0)
            {
                tre.addTre7Record(
                    rgn.addPolygons2(subdiv.polygon2())
                    );
            }
        }
    }
    tre.finalizeSubdivs();

    // 1st run with placeholders to create offset and sizes
    hdrGmp.write(file);
    file.write(copyrightData1);
    tre.writeHdr(file);
    file.write(copyrightData2);
    rgn.writeHdr(file);
    lbl.writeHdr(file);

    tre.writeData(file);
    rgn.writeData(file);
    lbl.writeData(file);

    // set offsets and sizes
    hdrGmp.setTre(tre);
    hdrGmp.setRgn(rgn);
    hdrGmp.setLbl(lbl);

    // 2nd run to write headers with offsets
    hdrGmp.write(file);
    tre.writeHdr(file);
    rgn.writeHdr(file);
    lbl.writeHdr(file);

    file.close();
}

void CFileGmp::remove(const QDir& path)
{
    QFile::remove(path.absoluteFilePath(name + ".gmp"));
}


