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

#include <tuple>

#include "headers/CHdrLbl.h"
#include "sections/CLbl1.h"
#include "sections/CLbl12.h"
#include "sections/CLbl28.h"
#include "sections/CLbl29.h"
#include "sections/CSection.h"

class CFileLbl {
 public:
  CFileLbl();
  virtual ~CFileLbl() = default;

  const CHdrLbl& header() const { return hdrLbl; }

  void writeHdr(QFile& file);
  void writeData(QFile& file);

  quint32 addLabel(const QString& str);
  std::tuple<quint32, quint32, quint32> addTile(const QString& path);

 private:
  CHdrLbl hdrLbl;

  CLbl1 lbl1;
  CSection lbl2;
  CSection lbl3;
  CSection lbl4;
  CSection lbl5;
  CSection lbl6;
  CSection lbl7;
  CSection lbl8;
  CSection lbl9;
  CSection lbl10;

  CSection lbl11;
  CLbl12 lbl12;
  CSection lbl13;
  CSection lbl14;
  CSection lbl15;
  CSection lbl16;
  CSection lbl17;
  CSection lbl18;
  CSection lbl19;
  CSection lbl20;

  CSection lbl21;
  CSection lbl22;
  CSection lbl23;
  CSection lbl24;
  CSection lbl25;
  CSection lbl26;
  CSection lbl27;
  CLbl28 lbl28;
  CLbl29 lbl29;
  CSection lbl30;

  CSection lbl31;
  CSection lbl32;
  CSection lbl33;
  CSection lbl34;
  CSection lbl35;
  CSection lbl36;
  CSection lbl37;
};
