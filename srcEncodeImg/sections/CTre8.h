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

/** Table of extended items visiblility

  This table keeps the maximum zoom level (minimum resolution) of each
  extended item. Each item has an entry. First lines next areas and
  last points.

  The encoder has to call the add... methods each time it adds and
  item to the rgn file.

*/
class CTre8 : public ISection {
 public:
  CTre8();
  virtual ~CTre8() = default;

  void write(QFile& file) override;

  void addPolyline(quint32 type, quint8 maplevel) { polylines_[type] << maplevel; }
  void addPolygon(quint32 type, quint8 maplevel) { polygons_[type] << maplevel; }
  void addPoint(quint32 type, quint8 maplevel) { points_[type] << maplevel; }

  quint16 numPolylines() const { return polylines_.size(); }
  quint16 numPolygons() const { return polygons_.size(); }
  quint16 numPoints() const { return points_.size(); }

 private:
  QMap<quint32, QSet<quint8> > polylines_;
  QMap<quint32, QSet<quint8> > polygons_;
  QMap<quint32, QSet<quint8> > points_;
};
