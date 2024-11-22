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

#include "CFileLbl.h"

#include "CException.h"

CFileLbl::CFileLbl() : lbl3(6), lbl4(7), lbl8(3), lbl9(6), lbl10(4), lbl11(3), lbl15(0x2e), lbl19(0xC), lbl20(3) {}

void CFileLbl::writeHdr(QFile& file) {
  hdrLbl.setLbl1(lbl1);
  hdrLbl.setLbl2(lbl2);
  hdrLbl.setLbl3(lbl3);
  hdrLbl.setLbl4(lbl4);
  hdrLbl.setLbl5(lbl5);
  hdrLbl.setLbl6(lbl6);
  hdrLbl.setLbl7(lbl7);
  hdrLbl.setLbl8(lbl8);
  hdrLbl.setLbl9(lbl9);
  hdrLbl.setLbl10(lbl10);

  hdrLbl.setLbl11(lbl11);
  hdrLbl.setLbl12(lbl12);
  hdrLbl.setLbl13(lbl13);
  hdrLbl.setLbl14(lbl14);
  hdrLbl.setLbl15(lbl15);
  hdrLbl.setLbl16(lbl16);
  hdrLbl.setLbl17(lbl17);
  hdrLbl.setLbl18(lbl18);
  hdrLbl.setLbl19(lbl19);
  hdrLbl.setLbl20(lbl20);

  hdrLbl.setLbl21(lbl21);
  hdrLbl.setLbl22(lbl22);
  hdrLbl.setLbl23(lbl23);
  hdrLbl.setLbl24(lbl24);
  hdrLbl.setLbl25(lbl25);
  hdrLbl.setLbl26(lbl26);
  hdrLbl.setLbl27(lbl27);
  hdrLbl.setLbl28(lbl28);
  hdrLbl.setLbl29(lbl29);
  hdrLbl.setLbl30(lbl30);

  hdrLbl.setLbl31(lbl31);
  hdrLbl.setLbl32(lbl32);
  hdrLbl.setLbl33(lbl33);
  hdrLbl.setLbl34(lbl34);
  hdrLbl.setLbl35(lbl35);
  hdrLbl.setLbl36(lbl36);
  hdrLbl.setLbl37(lbl37);

  hdrLbl.write(file);
}

void CFileLbl::writeData(QFile& file) {
  lbl12.write(file);
  lbl1.write(file);
  lbl2.write(file);
  lbl3.write(file);
  lbl4.write(file);
  lbl5.write(file);
  lbl6.write(file);
  lbl7.write(file);
  lbl8.write(file);
  lbl9.write(file);
  lbl10.write(file);

  lbl11.write(file);
  lbl13.write(file);
  lbl14.write(file);
  lbl15.write(file);
  lbl16.write(file);
  lbl17.write(file);
  lbl18.write(file);
  lbl19.write(file);
  lbl20.write(file);

  lbl21.write(file);
  lbl22.write(file);
  lbl23.write(file);
  lbl24.write(file);
  lbl25.write(file);
  lbl26.write(file);
  lbl27.write(file);
  lbl28.write(file);
  lbl29.write(file);
  lbl30.write(file);

  lbl31.write(file);
  lbl32.write(file);
  lbl33.write(file);
  lbl34.write(file);
  lbl35.write(file);
  lbl36.write(file);
  lbl37.write(file);
}

quint32 CFileLbl::addLabel(const QString& str) {
  quint32 offset = lbl1.add(str);
  if (offset > 0x3FFFFF) {
    throw CException("Label section exceeds maximum size.");
  }

  return offset;
}

std::tuple<quint32, quint32, quint32> CFileLbl::addTile(const QString& path) {
  auto [offsetTile, sizeTile] = lbl29.add(path);
  quint32 indexTile = lbl28.add(offsetTile);
  quint32 offsetLabel = lbl1.add(QFileInfo(path).fileName().toUpper());
  return {indexTile, sizeTile, offsetLabel};
}
