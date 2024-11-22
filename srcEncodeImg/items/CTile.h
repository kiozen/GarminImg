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

#include "items/CPolyline.h"

class CTile : public CPolyline {
 public:
  CTile(quint32 indexTile, quint32 sizeTile, quint32 indexLabel);
  virtual ~CTile() = default;

  void setBoundaries(qreal northbound, qreal eastbound, qreal southbound, qreal westbound);

  QByteArray encode(const CSubdiv &subdiv) const;

 private:
  quint32 indexTile_ = 0;
  quint32 indexLabel_ = 0;
  quint32 sizeTile_ = 0;

  qreal northbound_ = 0;
  qreal eastbound_ = 0;
  qreal southbound_ = 0;
  qreal westbound_ = 0;
};
