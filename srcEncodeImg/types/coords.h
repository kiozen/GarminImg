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

#include <QPointF>
#include <QtCore>
#include <limits>

using mapunit_t = qint32;

inline mapunit_t degToMapUnit(qreal deg) {
  constexpr qreal DELTA = 360.0 / (1 << 24) / 2;
  if (deg > 0) {
    return (int)((deg + DELTA) * (1 << 24) / 360);
  } else {
    return (int)((deg - DELTA) * (1 << 24) / 360);
  }
}

class CCoord {
 public:
  CCoord() {}
  CCoord(const QPointF& pt) {
    lat = degToMapUnit(pt.y());
    lon = degToMapUnit(pt.x());
  }
  ~CCoord() = default;

  const QPointF& operator=(const QPointF& pt) {
    lat = degToMapUnit(pt.y());
    lon = degToMapUnit(pt.x());
    return pt;
  }

  qint32 getId() const { return 0; }
  bool isNumberNode() const { return false; }

  mapunit_t getLatitude() const { return lat; }
  mapunit_t getLongitude() const { return lon; }

 private:
  mapunit_t lat = 0;
  mapunit_t lon = 0;
};
