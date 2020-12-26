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

#include <sections/ISection.h>

/** Table of offsets for extended items

  For each subdivision there is an entry in this table. If all sub-divisions
  of a sub-file contain no points or lines the record entry size can be reduced.

 */
class CTre7 : public ISection
{
public:
    CTre7();
    virtual ~CTre7() = default;

    void setSizeRecord(quint16 sizeRecord){sizeRecord_ = sizeRecord;}
    void addRecord(quint32 offsetPolygon2);
    void addRecord(quint32 offsetPolygon2, quint32 offsetPolyline2);
    void addRecord(quint32 offsetPolygon2, quint32 offsetPolyline2, quint32 offsetPoint2);

private:
    QDataStream stream {&data_, QIODevice::WriteOnly};
};


