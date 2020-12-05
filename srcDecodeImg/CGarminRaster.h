/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler oliver.eichler@gmx.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

   Garmin and MapSource are registered trademarks or trademarks of Garmin Ltd.
   or one of its subsidiaries.

   This source is based on John Mechalas documentation "Garmin IMG File Format" found
   at sourceforge. The missing bits and error were rectified by the source code of
   Konstantin Galichsky (kg@geopainting.com), http://www.geopainting.com

**********************************************************************************************/
#pragma once

#include <QtCore>

class CGarminRaster
{
public:
    CGarminRaster();
    virtual ~CGarminRaster();

    quint32 decode(const quint8 * pData, const quint8 * pEnd);

    double lon1;
    double lat1;
    double lon2;
    double lat2;

    quint32 index;
    quint32 size;

private:
    quint32 getBlockLength(quint8 val);
};


