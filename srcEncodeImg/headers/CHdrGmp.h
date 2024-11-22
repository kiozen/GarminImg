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

#include "headers/IHdrSubfile.h"

class CFileTre;
class CFileLbl;
class CFileRgn;
class CHdrRgn;
class CHdrLbl;

class CHdrGmp : public IHdrSubfile {
 public:
  CHdrGmp();
  virtual ~CHdrGmp() = default;

  void write(QFile &file) override;

  void setTre(const CFileTre &tre);
  void setRgn(const CFileRgn &rgn);
  void setLbl(const CFileLbl &lbl);

 protected:
#pragma pack(1)
  struct data_t {
    quint8 byte0x00000015_0x00000018[4] = {0};
    quint32 offsetTRE = 0;
    quint32 offsetRGN = 0;
    quint32 offsetLBL = 0;
    quint32 offsetNET = 0;
    quint32 offsetNOD = 0;
    quint32 offsetDEM = 0;
    quint8 byte0x00000031_0x00000034[4] = {0};
  };
#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
  data_t data;
};
