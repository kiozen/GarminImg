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

#include "CFileRgn.h"

#include <QtCore>
#include <QtGui>

CFileRgn::CFileRgn() {}

void CFileRgn::writeHdr(QFile& file) {
  hdrRgn.setRgn1(rgn1);
  hdrRgn.setRgn2(rgn2);
  hdrRgn.setRgn3(rgn3);
  hdrRgn.setRgn4(rgn4);
  hdrRgn.setRgn5(rgn5);

  hdrRgn.write(file);
}

void CFileRgn::writeData(QFile& file) {
  rgn1.write(file);
  rgn2.write(file);
  rgn3.write(file);
  rgn4.write(file);
  rgn5.write(file);
}
