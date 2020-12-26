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

#include "items/IItem.h"
#include <QPolygonF>


class CPolyline : public QPolygonF, public IItem
{
public:
    CPolyline(quint32 type);
    virtual ~CPolyline() = default;

    bool isRoad() const {return false;}
    bool hasInternalNodes() const {return false;}
    bool isLastSegment() const {return true;}
    bool hasHouseNumbers() const {return false;}

    bool isPolygon() const {return false;}
};

