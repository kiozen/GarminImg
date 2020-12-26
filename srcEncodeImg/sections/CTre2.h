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

#include "types/CSubdiv.h"
#include "sections/ISection.h"

class CSubdiv;

/** Table of sub-divisions

  All subdivisions ordered by map levels. Less resolution levels
  first. Highest resolution level last. addSubdiv() has to be
  called in the right order.

  This is just encoding the sub-divs. The data structures are part
  of the maplevels.

 */
class CTre2 : public ISection
{
public:
    CTre2();
    virtual ~CTre2() = default;

    void addSubdiv(const CSubdiv& subdiv);
};
