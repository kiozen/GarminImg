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

#include "sections/CTre3.h"

CTre3::CTre3()
{
    sizeRecord_ = 3;
}

void CTre3::add(quint32 lbl1Offset)
{
    data_.append(lbl1Offset & 0x0FF);
    data_.append((lbl1Offset >> 8) & 0x0FF);
    data_.append((lbl1Offset >> 16) & 0x0FF);
}

