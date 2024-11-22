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

#include "CKeyGen.h"

CKeyGen::CKeyGen(quint32 mapId, qint32 sizeHdrTre) : mapId(mapId), sizeHdrTre(sizeHdrTre) {}

void CKeyGen::calcThird() {
  constexpr quint8 mapIdCodeTable[] = {0, 1, 0xf, 5, 0xd, 4, 7, 6, 0xb, 9, 0xe, 8, 2, 0xa, 0xc, 3};

  for (int i = 0; i < 8; i++) {
    int n = mapIdDigit(i);
    values[2][(i ^ 1)] = mapIdCodeTable[n];
  }
}

void CKeyGen::calcFourth() { memcpy(values[3], values[2], sizeof(values[3])); }

void CKeyGen::calcFirst() {
  // First bytes are the low bytes of the mapId, with the corresponding
  // value from value[3] added.
  values[0][0] = (quint8)(mapIdDigit(4) + values[3][0]);
  values[0][1] = (quint8)(mapIdDigit(5) + values[3][1]);
  values[0][2] = (quint8)(mapIdDigit(6) + values[3][2]);
  values[0][3] = (quint8)(mapIdDigit(7) + values[3][3]);

  // Copies of v3
  values[0][4] = values[3][4];
  values[0][5] = values[3][5];
  values[0][6] = values[3][6];

  // Always (?) one more.  The one likely comes from some other
  // part of the header, but we don't know if or where.
  values[0][7] = (quint8)(values[3][7] + 1);
}

void CKeyGen::calcSecond() {
  // Just same as in v3
  values[1][0] = values[3][0];
  values[1][1] = values[3][1];

  int h1 = sizeHdrTre >> 4;
  int h2 = sizeHdrTre;
  values[1][2] = (quint8)((values[3][2] + h1) & 0xf);
  values[1][3] = (quint8)((values[3][3] + h2) & 0xf);

  // The following are the sum of individual nibbles in U3 and the
  // corresponding nibble in the top half of mapId.
  values[1][4] = (quint8)(values[3][4] + mapIdDigit(0));
  values[1][5] = (quint8)(values[3][5] + mapIdDigit(1));
  values[1][6] = (quint8)(values[3][6] + mapIdDigit(2));
  values[1][7] = (quint8)(values[3][7] + mapIdDigit(3));
}

void CKeyGen::addOffset() {
  constexpr int offsetMap[] = {6, 7, 5, 11, 3, 10, 13, 12, 1, 15, 4, 14, 8, 0, 2, 9};

  // To get the offset value we add up all the even nibbles of the map
  // number and transform via a table.
  int n = mapIdDigit(1) + mapIdDigit(3) + mapIdDigit(5) + mapIdDigit(7);

  int offset = offsetMap[n & 0xf];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++) {
      values[i][j] += offset;
    }
  }
}

void CKeyGen::calcKey(quint8 *target) {
  calcThird();
  calcFourth();
  calcFirst();
  calcSecond();
  addOffset();

  quint32 *tmp = (quint32 *)target;
  *tmp++ = value(0);
  *tmp++ = value(1);
  *tmp++ = value(2);
  *tmp++ = value(3);
}

quint32 CKeyGen::value(qint32 n) {
  quint32 res = 0;
  for (int i = 0; i < 8; i++) {
    res |= ((values[n][i] & 0xf) << (4 * (7 - i)));
  }
  return res;
}
