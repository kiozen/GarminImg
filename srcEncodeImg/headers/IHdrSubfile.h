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

using quint24 = quint8[3];

class IHdrSubfile {
 public:
  IHdrSubfile(quint16 size, const QString& typeSpecifier);
  virtual ~IHdrSubfile() = default;

  virtual void write(QFile& file);
  quint32 offset() const;
  quint32 size() const;

 protected:
  quint64 offset_ = 0;
  quint64 size_ = 0;

 private:
#pragma pack(1)
  struct common_t {
    quint16 size;
    char type[10];  ///< 0x00000002 .. 0x0000000B
    const quint8 byte0x0000000C = 1;
    const quint8 flag = 0;  ///< 0x0000000D
    qint16 year;
    qint8 month;
    qint8 day;
    qint8 hour;
    qint8 minute;
    qint8 second;
  };
#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
  common_t common_;
};
