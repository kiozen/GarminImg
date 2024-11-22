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

#include "types/coords.h"

class CTile;
class CMapLevel;

class CSubdiv {
 public:
  using number_t = qint16;

  CSubdiv(const CMapLevel& maplevel, quint16 n, qreal northbound, qreal eastbound, qreal southbound, qreal westbound);
  virtual ~CSubdiv() = default;

  quint8 zoom() const { return zoom_; }
  quint8 shift() const { return shift_; }

  mapunit_t roundLatToLocalShifted(mapunit_t absval) const;
  mapunit_t roundLonToLocalShifted(mapunit_t absval) const;
  const CCoord& center() const { return center_; }

  void addTile(const CTile& tile);

  qreal northbound() const { return northbound_; }
  qreal eastbound() const { return eastbound_; }
  qreal southbound() const { return southbound_; }
  qreal westbound() const { return westbound_; }

  void setNext(number_t n) { next_ = n; }
  number_t number() const { return n_; }

  QByteArray encode() const;

  const QByteArray& polygon2() { return polygon2_; }
  const QByteArray& polyline2() { return polyline2_; }
  const QByteArray& point2() { return point2_; }

 private:
  qreal northbound_;
  qreal eastbound_;
  qreal southbound_;
  qreal westbound_;

  quint32 rgnOffset_ = 0;
  quint8 elements_ = 0;

  CCoord center_;
  quint8 zoom_ = 0;
  quint8 shift_ = 0;

  number_t next_ = -1;
  number_t n_ = -1;

  mapunit_t width_ = 0;
  mapunit_t height_ = 0;

  QByteArray polygon2_;
  QByteArray polyline2_;
  QByteArray point2_;
};
