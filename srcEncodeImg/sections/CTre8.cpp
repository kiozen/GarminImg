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

#include "sections/CTre8.h"

#include <algorithm>

CTre8::CTre8() { sizeRecord_ = 3; }

void CTre8::write(QFile& file) {
  data_.clear();
  QDataStream stream(&data_, QIODevice::WriteOnly);
  stream.setByteOrder(QDataStream::LittleEndian);

  for (const quint32 extType : polylines_.keys()) {
    quint8 type = (extType >> 8) & 0x0FF;
    quint8 subtype = extType & 0x0FF;
    quint8 maxlevel = *std::max_element(polylines_[extType].begin(), polylines_[extType].end());
    stream << type << maxlevel << subtype;
  }

  for (const quint32 extType : polygons_.keys()) {
    quint8 type = (extType >> 8) & 0x0FF;
    quint8 subtype = extType & 0x0FF;
    quint8 maxlevel = *std::max_element(polygons_[extType].begin(), polygons_[extType].end());
    stream << type << maxlevel << subtype;
  }

  for (const quint32 extType : points_.keys()) {
    quint8 type = (extType >> 8) & 0x0FF;
    quint8 subtype = extType & 0x0FF;
    quint8 maxlevel = *std::max_element(points_[extType].begin(), points_[extType].end());
    stream << type << maxlevel << subtype;
  }

  ISection::write(file);
}
