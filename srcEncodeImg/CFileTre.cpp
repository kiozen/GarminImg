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

#include "CFileTre.h"

#include "CException.h"
#include "helpers/CKeyGen.h"

CFileTre::CFileTre(const QString& mapNumber) : tre4(2), tre5(2), tre6(3), mapId(mapNumber.toUInt()) {}

CSubdiv& CFileTre::addSubdiv(quint8 zoom, qreal northbound, qreal eastbound, qreal southbound, qreal westbound) {
  CMapLevel& maplevel = tre1.maplevel(zoom);
  maplevel.subdivs() << CSubdiv(maplevel, ++cntSubdiv_, northbound, eastbound, southbound, westbound);

  northbound_ = qMax(northbound_, northbound);
  eastbound_ = qMax(eastbound_, eastbound);
  southbound_ = qMin(southbound_, southbound);
  westbound_ = qMin(westbound_, westbound);

  return maplevel.subdivs().last();
}

CSubdiv& CFileTre::subdivByNumber(CSubdiv::number_t number) {
  for (CMapLevel& maplevel : tre1.maplevels()) {
    for (CSubdiv& subdiv : maplevel.subdivs()) {
      if (subdiv.number() == number) {
        return subdiv;
      }
    }
  }

  throw CException("Unknown subdiv number " + QString::number(number));
}

void CFileTre::addTre7Record(quint32 offsetPolygon2) { tre7.addRecord(offsetPolygon2); }

void CFileTre::addTre7Record(quint32 offsetPolygon2, quint32 offsetPolyline2) {
  tre7.addRecord(offsetPolygon2, offsetPolyline2);
}

void CFileTre::addTre7Record(quint32 offsetPolygon2, quint32 offsetPolyline2, quint32 offsetPoint2) {
  tre7.addRecord(offsetPolygon2, offsetPolyline2, offsetPoint2);
}

void CFileTre::addTre8Polygon(quint32 type, quint8 maplevel) {
  tre8.addPolygon(type, maplevel);
  hdrTre.setNumExtPolygons(tre8.numPolygons());
}

void CFileTre::finalizeSubdivs() {
  for (CMapLevel& maplevel : tre1.maplevels()) {
    for (CSubdiv& subdiv : maplevel.subdivs()) {
      tre2.addSubdiv(subdiv);
    }
  }
}

void CFileTre::writeHdr(QFile& file) {
  hdrTre.setTr1(tre1);
  hdrTre.setTr2(tre2);
  hdrTre.setTr3(tre3);
  hdrTre.setTr4(tre4);
  hdrTre.setTr5(tre5);
  hdrTre.setTr6(tre6);
  hdrTre.setTr7(tre7);
  hdrTre.setTr8(tre8);
  hdrTre.setTr9(tre9);
  hdrTre.setTr10(tre10);

  hdrTre.setBoundary(northbound_, eastbound_, southbound_, westbound_);

  CKeyGen keyGen(mapId, hdrTre.size());
  keyGen.calcKey(hdrTre.key());

  hdrTre.write(file);
}

void CFileTre::writeData(QFile& file) {
  tre1.write(file);
  tre2.write(file);
  tre3.write(file);
  tre4.write(file);
  tre5.write(file);
  tre6.write(file);
  tre7.write(file);
  tre8.write(file);
  tre9.write(file);
  tre10.write(file);
}
