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

#include "sections/CTre1.h"


CTre1::CTre1()
{
}


CMapLevel& CTre1::maplevel(quint8 zoom)
{
    for(CMapLevel& maplevel : maplevels_)
    {
        if(maplevel.zoom() == zoom)
        {
            return maplevel;
        }
    }

    maplevels_ << CMapLevel(zoom, 24 - zoom);

    qSort(maplevels_);

    return maplevel(zoom);
}

void CTre1::write(QFile& file)
{
    offset_ = file.pos();

    bool first = true;
    for(CMapLevel& maplevel : maplevels_)
    {
        maplevel.write(file, first);
        file.flush();
    }

    size_ = file.pos() - offset_;
}


