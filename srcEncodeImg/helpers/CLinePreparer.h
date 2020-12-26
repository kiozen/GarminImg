/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2006 Steve Ratcliffe

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

#include "types/coords.h"
#include "helpers/CBitWriter.h"

#include <QVector>

class CPolyline;
class CSubdiv;

class CLinePreparer
{
public:
    CLinePreparer(const CPolyline &line, const CSubdiv &subdiv);
    virtual ~CLinePreparer() = default;

    /**
     * Write the bit stream to a BitWriter and return it.
     * Try different values for xBase and yBase to find the one
     * that results in the shortest bit stream.
     *
     * @return A class containing the written byte stream.
     */
    CBitWriter makeShortestBitStream(qint32 minPointsRequired);

    /**
     * Write the bit stream to a BitWriter and return it.
     *
     * @return A class containing the written byte stream.
     */
    CBitWriter makeBitStream(qint32 minPointsRequired, qint32 xb, qint32 yb);

private:
    /**
     * Calculate the deltas of one point to the other.  While we are doing
     * this we must save more information about the maximum sizes, if they
     * are all the same sign etc.  This must be done separately for both
     * the lat and long values.
     */
    void calcDeltas();
    /**
     * The bits needed to hold a number without truncating it.
     *
     * @param val The number for bit counting.
     * @return The number of bits required.
     */
    qint32 bitsNeeded(mapunit_t val);
    qint32 bits2Base(qint32 bits);
    qint32 base2Bits(qint32 base);


    const CPolyline& polyline;
    const CSubdiv& subdiv;

    bool extraBit = false;
    bool ignoreNumberOnlyNodes = true;
    bool extTypeLine = false;

    bool xSameSign = true;
    bool xSignNegative = false;     // Set if all negative

    bool ySameSign = true;
    bool ySignNegative = false;     // Set if all negative

    // The base number of bits
    qint32 xBase = 0;
    qint32 yBase = 0;

    // The delta changes between the points.
    QVector<mapunit_t> deltas;
    QVector<bool> nodes;
};


