/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2008 Steve Ratcliffe

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

#include <QtCore>

class CKeyGen {
 public:
  CKeyGen(quint32 mapId, qint32 sizeHdrTre);
  virtual ~CKeyGen() = default;

  void calcKey(quint8 *target);

 private:
  void calcThird();
  void calcFourth();
  void calcFirst();
  void calcSecond();
  void addOffset();
  quint32 value(qint32 n);

  int mapIdDigit(qint32 i) { return (mapId >> (4 * (7 - i))) & 0xf; }
  quint8 values[4][8] = {0};

  quint32 mapId = 0;
  qint32 sizeHdrTre = 0;
};
