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

#include <QMap>
#include <QPointF>

extern double MIN_DELTA;

inline bool operator<(const QPointF& p1, const QPointF& p2)
{
    if(p1.x() < p2.x())
    {
        return true;
    }
    if(p1.x() == p2.x())
    {
        return p1.y() < p2.y();
    }

    return false;
}

template <typename T>
class CFuzzyMap
{
public:
    CFuzzyMap<T>(){}
    virtual ~CFuzzyMap() = default;

    bool contains(const QPointF& p) const
    {
        for(const QPointF& key : map.keys())
        {
            if((key - p).manhattanLength() < MIN_DELTA)
            {
                return true;
            }
        }
        return false;
    }

    T& operator[](const QPointF& p)
    {
        for(const QPointF& key : map.keys())
        {
            if((key - p).manhattanLength() < MIN_DELTA)
            {
                return map[key];
            }
        }

        map[p] = T();
        return map[p];
    }

    QList<QPointF> keys() const
    {
        return map.keys();
    }

    int size() const
    {
        return map.size();
    }

private:
    QMap<QPointF, T> map;
};


