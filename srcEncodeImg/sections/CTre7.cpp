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

#include "sections/CTre7.h"

CTre7::CTre7() { stream.setByteOrder(QDataStream::LittleEndian); }

void CTre7::addRecord(quint32 offsetPolygon2) {
  Q_ASSERT_X(sizeRecord_ == 4, "addRecord", "Record size does not match");
  stream << offsetPolygon2;
}

void CTre7::addRecord(quint32 offsetPolygon2, quint32 offsetPolyline2) {
  Q_ASSERT_X(sizeRecord_ == 8, "addRecord", "Record size does not match");
  stream << offsetPolygon2;
  stream << offsetPolyline2;
}

void CTre7::addRecord(quint32 offsetPolygon2, quint32 offsetPolyline2, quint32 offsetPoint2) {
  Q_ASSERT_X(sizeRecord_ == 12, "addRecord", "Record size does not match");
  stream << offsetPolygon2;
  stream << offsetPolyline2;
  stream << offsetPoint2;
}
