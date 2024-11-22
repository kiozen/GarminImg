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

#include "headers/CHdrRgn.h"
#include "sections/CRgn2.h"
#include "sections/CRgn3.h"
#include "sections/CRgn4.h"
#include "sections/CSection.h"

class QByteArray;

class CFileRgn {
 public:
  CFileRgn();
  virtual ~CFileRgn() = default;

  const CHdrRgn& header() const { return hdrRgn; }

  quint32 addPolygons2(const QByteArray& polygons) { return rgn2.addPolygons2(polygons); }
  quint32 addPolylines2(const QByteArray& polylines) { return rgn3.addPolylines2(polylines); }
  quint32 addPoints2(const QByteArray& points) { return rgn4.addPoints2(points); }

  void writeHdr(QFile& file);
  void writeData(QFile& file);

 private:
  CHdrRgn hdrRgn;

  CSection rgn1;
  CRgn2 rgn2;
  CRgn3 rgn3;
  CRgn4 rgn4;
  CSection rgn5;
};
