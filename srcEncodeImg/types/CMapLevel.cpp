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

#include "CMapLevel.h"

CMapLevel::CMapLevel(quint8 zoom, quint8 bits)
    : zoom_(zoom)
    , bits_(bits)
{
}

void CMapLevel::write(QFile& file, bool& first)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    if(first)
    {
        stream << quint8(zoom_ |= 0x80);
        first = false;
    }
    else
    {
        stream << quint8(zoom_ &= ~0x80);
    }
    stream << quint8(bits_);
    stream << quint16(subdivs_.count());

    file.write(data);
}
