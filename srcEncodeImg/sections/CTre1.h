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

#include "types/CMapLevel.h"
#include "sections/ISection.h"

/** Table of map levels.

 */
class CTre1 : public ISection
{
public:
    CTre1();
    virtual ~CTre1() = default;

    CMapLevel& maplevel(quint8 zoom);
    void write(QFile& file) override;

    QList<CMapLevel>& maplevels()
    {
        return maplevels_;
    }

private:
    QList<CMapLevel> maplevels_;
};