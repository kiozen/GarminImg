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

#include "CException.h"
#include "sections/ISection.h"

ISection::ISection()
{
}

quint32 ISection::offset() const
{
    if(offset_ & 0xFFFFFFFF00000000LL)
    {
        throw CException("Offset of section exceeds 32 bit");
    }

    return offset_;
}

quint32 ISection::size() const
{
    if(size_ & 0xFFFFFFFF00000000LL)
    {
        throw CException("Size of section exceeds 32 bit");
    }

    return size_;
}

void ISection::write(QFile& file)
{
    offset_ = file.pos();
    file.write(data_);
    size_ = file.pos() - offset_;
}
