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

#include <QtCore>

class CMapLevel
{
public:
    CMapLevel(quint8 zoom, quint8 bits);
    virtual ~CMapLevel() = default;

    void write(QFile& file, bool &first);

    QList<CSubdiv>& subdivs()
    {
        return subdivs_;
    }

    quint8 zoom() const {return zoom_;}

    bool operator<(const CMapLevel& m) const
    {
        return m.zoom() < zoom_;
    }

    bool operator==(const CMapLevel& m) const
    {
        return m.zoom() == zoom_;
    }

private:    
    quint8 zoom_ = 0;
    quint8 bits_ = 0;
    QList<CSubdiv> subdivs_;
};


