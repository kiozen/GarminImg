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

#include "items/CTile.h"

#include "helpers/CBitWriter.h"
#include "helpers/CLinePreparer.h"
#include "types/CSubdiv.h"

CTile::CTile(quint32 indexTile, quint32 sizeTile, quint32 indexLabel)
    : CPolyline(0x10613), indexTile_(indexTile), sizeTile_(sizeTile), indexLabel_(indexLabel) {}

void CTile::setBoundaries(qreal northbound, qreal eastbound, qreal southbound, qreal westbound) {
  northbound_ = northbound;
  eastbound_ = eastbound;
  southbound_ = southbound;
  westbound_ = westbound;

  append(QPointF(westbound_, northbound_));
  append(QPointF(eastbound_, northbound_));
  append(QPointF(eastbound_, southbound));
  append(QPointF(westbound_, southbound));

  // qDebug() << *this;
}

#define GARMIN_DEG(x) ((x) < 0x800000 ? (double)(x)*360.0 / 16777216.0 : (double)((x)-0x1000000) * 360.0 / 16777216.0)

QByteArray CTile::encode(const CSubdiv& subdiv) const {
  CLinePreparer lp(*this, subdiv);

  const CCoord& p0 = first();
  mapunit_t deltaLon = subdiv.roundLonToLocalShifted(p0.getLongitude());
  mapunit_t deltaLat = subdiv.roundLatToLocalShifted(p0.getLatitude());

  QByteArray data;
  QDataStream stream(&data, QIODevice::WriteOnly);
  stream.setByteOrder(QDataStream::LittleEndian);

  quint32 t = type();
  t |= 0x20;  // has label
  t |= 0x80;  // has extra bytes information

  stream << quint8(t >> 8);
  stream << quint8(t);
  stream << quint16(deltaLon & 0xFFFF);
  stream << quint16(deltaLat & 0xFFFF);

  CBitWriter bw = lp.makeShortestBitStream(1);
  int blen = bw.length();
  Q_ASSERT_X(blen > 1, "blen", "zero length bitstream");
  Q_ASSERT_X(blen < 0x10000, "blen", "bitstream too long ");

  if (blen >= 0x7f) {
    stream << quint8((blen << 2) | 2);
    stream << quint8((blen << 2) >> 8);
  } else {
    stream << quint8((blen << 1) | 1);
  }

  stream.writeRawData(bw.data(), blen);

  if (indexLabel_ != 0) {
    stream << quint8(indexLabel_);
    stream << quint8(indexLabel_ >> 8);
    stream << quint8(indexLabel_ >> 16);
  }

  stream << quint8(0xE0);  // extended data type
  stream << quint8(45);    // length of extended data
  stream << quint16(indexTile_);
  stream << quint32(degToMapUnit(northbound_) << 8);
  stream << quint32(degToMapUnit(eastbound_) << 8);
  stream << quint32(degToMapUnit(southbound_) << 8);
  stream << quint32(degToMapUnit(westbound_) << 8);
  stream << quint32(sizeTile_);

  return data;
}
