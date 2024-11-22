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

#include <QtCore>

class CFatBlock {
 public:
  CFatBlock();
  CFatBlock(const QFileInfo& info);
  virtual ~CFatBlock() = default;

  static quint32 size() { return sizeof(data_t); }
  static void setBlocksize(quint32 size) { blocksize_ = size; }
  static quint32 blockcount() { return blockcount_; }
  void enumerate();
  void writeBlock(QFile& file);
  void writeData(QFile& file);

 private:
  friend class CFileImg;
  static quint32 blocksize_;
  static quint32 blockcount_;
#pragma pack(1)
  struct data_t {
    quint8 flag = 1;        ///< 0x00000000
    char name[8] = {0x20};  ///< 0x00000001 .. 0x00000008
    char type[3] = {0x20};  ///< 0x00000009 .. 0x0000000B
    quint32 size = 0;       ///< 0x0000000C .. 0x0000000F
    quint16 part = 0;       ///< 0x00000010 .. 0x00000011
    quint8 byte0x00000012_0x0000001F[14] = {0x00};
    quint16 blocks[240] = {0xFF};  ///< 0x00000020 .. 0x000001FF
  };
#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
  data_t data_;
  QFileInfo fileinfo_;
};
