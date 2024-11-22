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

#include "types/CSubdiv.h"

#include "../srcDecodeImg/CGarminPolygon.h"
#include "items/CTile.h"
#include "types/CMapLevel.h"

CSubdiv::CSubdiv(const CMapLevel& maplevel, quint16 n, qreal northbound, qreal eastbound, qreal southbound,
                 qreal westbound)
    : northbound_(northbound),
      eastbound_(eastbound),
      southbound_(southbound),
      westbound_(westbound),
      zoom_(maplevel.zoom()),
      shift_(maplevel.zoom()),
      n_(n)

{
  center_ = QPointF((westbound_ + eastbound_) / 2, (northbound_ + southbound_) / 2);

  width_ = degToMapUnit(eastbound_ - westbound_);
  height_ = degToMapUnit(northbound_ - southbound_);
}

mapunit_t CSubdiv::roundLatToLocalShifted(mapunit_t absval) const {
  mapunit_t val = absval - center_.getLatitude();
  val += ((1 << shift_) / 2);

  mapunit_t deltaLat = val >> shift_;
  Q_ASSERT_X(deltaLat >= -0x8000 && deltaLat <= 0x7fff, "roundLatToLocalShifted", "deltaLat = " + deltaLat);
  return deltaLat;
}

mapunit_t CSubdiv::roundLonToLocalShifted(mapunit_t absval) const {
  mapunit_t val = absval - center_.getLongitude();
  val += ((1 << shift_) / 2);

  mapunit_t deltaLon = val >> shift_;
  Q_ASSERT_X(deltaLon >= -0x8000 && deltaLon <= 0x7fff, "roundLonToLocalShifted", "deltaLon = " + deltaLon);
  return deltaLon;
}

void CSubdiv::addTile(const CTile& tile) {
  const QByteArray& data = tile.encode(*this);
  polygon2_ += data;

  //    CGarminPolygon p;
  //    p.decode2(center_.getLongitude(), center_.getLatitude(), shift_, false, (const quint8*)data.data(), (const
  //    quint8*)data.data() + data.size());
}

QByteArray CSubdiv::encode() const {
  QByteArray data;
  QDataStream stream(&data, QIODevice::WriteOnly);
  stream.setByteOrder(QDataStream::LittleEndian);

  stream << quint8(rgnOffset_ & 0x0FF) << quint8((rgnOffset_ >> 8) & 0x0FF) << quint8((rgnOffset_ >> 16) & 0x0FF);
  stream << elements_;

  mapunit_t centerLon = center_.getLongitude();
  mapunit_t centerLat = center_.getLatitude();
  stream << quint8(centerLon & 0x0FF) << quint8((centerLon >> 8) & 0x0FF) << quint8((centerLon >> 16) & 0x0FF);
  stream << quint8(centerLat & 0x0FF) << quint8((centerLat >> 8) & 0x0FF) << quint8((centerLat >> 16) & 0x0FF);

  quint16 terminate = 0x8000;  // todo: right now  we always terminate
  stream << quint16(width_ | terminate);
  stream << quint16(height_);
  if (next_ > 0) {
    stream << next_;
  }

  return data;
}
