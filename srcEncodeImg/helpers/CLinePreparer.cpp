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

#include "helpers/CLinePreparer.h"
#include "items/CPolyline.h"
#include "types/CSubdiv.h"

CLinePreparer::CLinePreparer(const CPolyline &line, const CSubdiv& subdiv)
    : polyline(line)
    , subdiv(subdiv)
{
    extraBit = line.isRoad() && subdiv.zoom() == 0
               && (line.hasInternalNodes() || !line.isLastSegment());
    ignoreNumberOnlyNodes = !line.hasHouseNumbers();
    extTypeLine = line.hasExtendedType();

    calcDeltas();
}



void CLinePreparer::calcDeltas()
{
    // Space to hold the deltas
    qint32 numPointsToUse = polyline.size();
    if (polyline.isPolygon() && polyline.first() == polyline.last())
    {
        --numPointsToUse; // no need to write the closing point
    }
    deltas.resize(2 * (numPointsToUse - 1));
    deltas.fill(0);

    if (extraBit)
    {
        nodes.resize(numPointsToUse);
        nodes.fill(false);
    }
    bool first = true;

    // OK go through the points
    mapunit_t lastLat = 0;
    mapunit_t lastLong = 0;
    mapunit_t minDx = INT_MAX;
    mapunit_t maxDx = 0;
    mapunit_t minDy = INT_MAX;
    mapunit_t maxDy = 0;
    // index of first point in a series of identical coords (after shift)
    qint32 firstsame = 0;
    for (qint32 i = 0; i < numPointsToUse; i++)
    {
        CCoord co = polyline[i];

        mapunit_t lat = subdiv.roundLatToLocalShifted(co.getLatitude());
        mapunit_t lon = subdiv.roundLonToLocalShifted(co.getLongitude());
        mapunit_t dx = lon - lastLong;
        mapunit_t dy = lat - lastLat;
        lastLong = lon;
        lastLat = lat;
        if (first)
        {
            first = false;
            continue;
        }

        /*
         * Current thought is that the node indicator is set when
         * the point is a routing node or a house number node.
         * There's a separate first extra bit
         * that always appears to be false. The last points' extra bit
         * is set if the point is a node and this is not the last
         * polyline making up the road.
         */
        if (extraBit)
        {
            bool isSpecialNode = co.getId() > 0 || (!ignoreNumberOnlyNodes && co.isNumberNode());
            if (dx != 0 || dy != 0 || isSpecialNode)
            {
                firstsame = i;
            }
            bool extra = false;
            if (isSpecialNode)
            {
                if (i < nodes.size() - 1)
                {
                    // inner node of polyline
                    extra = true;
                }
                else
                {
                    // end node of polyline: set if inner
                    // node of road
                    extra = !polyline.isLastSegment();
                }
            }

            /*
             * Only the first among a range of equal points
             * is written, so set the bit if any of the points
             * is a node.
             * Since we only write extra bits at level 0 now,
             * this can only happen when points in the input
             * data round to the same point in map units, so
             * it may be better to handle this in the
             * reader.
             */
            nodes[firstsame] = nodes[firstsame] || extra;
        }

        // find largest delta values
        if (dx < minDx)
        {
            minDx = dx;
        }
        if (dx > maxDx)
        {
            maxDx = dx;
        }
        if (dy < minDy)
        {
            minDy = dy;
        }
        if (dy > maxDy)
        {
            maxDy = dy;
        }

        // Save the deltas
        deltas[2 * (i - 1)] = dx;
        deltas[2 * (i - 1) + 1] = dy;
    }
    // Find the maximum number of bits required to hold the delta values.
    qint32 xBits = qMax(bitsNeeded(minDx), bitsNeeded(maxDx));
    qint32 yBits = qMax(bitsNeeded(minDy), bitsNeeded(maxDy));

    // Now we need to know the 'base' number of bits used to represent
    // the value.  In decoding you start with that number and add various
    // adjustments to get the final value.  We need to try and work
    // backwards from this.
    //
    // Note that the sign bit is already not included so there is
    // no adjustment needed for it.

    xBase = bits2Base(xBits);
    yBase = bits2Base(yBits);

    // Set flags for same sign etc.
    xSameSign = !(minDx < 0 && maxDx > 0);
    ySameSign = !(minDy < 0 && maxDy > 0);
    if (xSameSign)
    {
        xSignNegative = minDx < 0;
    }
    if (ySameSign)
    {
        ySignNegative = minDy < 0;
    }
}

CBitWriter CLinePreparer::makeBitStream(qint32 minPointsRequired, qint32 xb, qint32 yb)
{
    Q_ASSERT(xb >= 0 && yb >= 0);

    qint32 xbits = base2Bits(xb);
    if (!xSameSign)
    {
        xbits++;
    }
    qint32 ybits = base2Bits(yb);
    if (!ySameSign)
    {
        ybits++;
    }

    // Write the bitstream
    CBitWriter bw;

    // Pre bit stream info
    bw.putn(xb, 4);
    bw.putn(yb, 4);

    bw.put1(xSameSign);
    if (xSameSign)
    {
        bw.put1(xSignNegative);
    }

    bw.put1(ySameSign);
    if (ySameSign)
    {
        bw.put1(ySignNegative);
    }

    if(extTypeLine)
    {
        bw.put1(false);         // no extra bits required
    }

    // first extra bit always appears to be false
    // refers to the start point?
    if (extraBit)
    {
        bw.put1(false);
    }

    qint32 numPointsEncoded = 1;
    for (qint32 i = 0; i < deltas.length(); i += 2)
    {
        qint32 dx = deltas[i];
        qint32 dy = deltas[i + 1];
        if (dx == 0 && dy == 0)
        {
            if (extraBit && !nodes[i / 2 + 1] && i + 2 != deltas.length()) // don't skip CoordNode
            {
                continue;
            }
        }
        ++numPointsEncoded;

        if (xSameSign)
        {
            bw.putn(qAbs(dx), xbits);
        }
        else
        {
            bw.sputn(dx, xbits);
        }

        if (ySameSign)
        {
            bw.putn(qAbs(dy), ybits);
        }
        else
        {
            bw.sputn(dy, ybits);
        }
        if (extraBit)
        {
            bw.put1(nodes[i / 2 + 1]);
        }
    }

    Q_ASSERT(numPointsEncoded >= minPointsRequired);

    return bw;
}

CBitWriter CLinePreparer::makeShortestBitStream(qint32 minPointsRequired)
{
    CBitWriter bsSimple = makeBitStream(minPointsRequired, xBase, yBase);
    return bsSimple;

//    CBitWriter bsBest = bsSimple;
//    qint32 xBestBase = xBase;
//    qint32 yBestBase = yBase;
//    if (xBase > 0)
//    {
//        qint32 notBetter = 0;
//        bool xSameSignBak = xSameSign;
//        xSameSign = false;
//        for (qint32 xTestBase = xBase - 1; xTestBase >= 0; xTestBase--)
//        {
//            CBitWriter bstest = makeBitStream(minPointsRequired, xTestBase, yBase);
//            if (bstest.getBitPosition() >= bsBest.getBitPosition() )
//            {
//                if (++notBetter >= 2)
//                {
//                    break; // give up
//                }
//            }
//            else
//            {
//                xBestBase = xTestBase;
//                bsBest = bstest;
//                xSameSignBak = false;
//            }
//        }
//        xSameSign = xSameSignBak;
//    }
//    if (yBase > 0)
//    {
//        qint32 notBetter = 0;
//        bool ySameSignBak = ySameSign;
//        ySameSign = false;
//        for (qint32 yTestBase = yBase - 1; yTestBase >= 0; yTestBase--)
//        {
//            CBitWriter bstest = makeBitStream(minPointsRequired, xBestBase, yTestBase);
//            if (bstest.getBitPosition() >= bsBest.getBitPosition())
//            {
//                if (++notBetter >= 2)
//                {
//                    break; // give up
//                }
//            }
//            else
//            {
//                yBestBase = yTestBase;
//                bsBest = bstest;
//                ySameSignBak = false;
//            }
//        }
//        ySameSign = ySameSignBak;
//    }
//    if (bsSimple.length() == bsBest.length())
//    {
//        // if the (byte) length was not improved,
//        // prefer the bit stream that doesn't need the special "trick"
//        // to encode large values, it is assumed that this can safe a
//        // few CPU cycles when reading the map
//        return bsSimple;
//    }
//    return bsBest;
}


qint32 CLinePreparer::bitsNeeded(mapunit_t val)
{
    quint32 n = qAbs(val);

    qint32 count = 0;
    while (n != 0)
    {
        n >>= 1;
        count++;
    }
    return count;
}

qint32 CLinePreparer::bits2Base(qint32 bits)
{
    qint32 base = qMax(0, bits - 2);
    if (base > 10)
    {
        if ((base & 0x1) == 0)
        {
            base++;
        }
        base = 9 + (base - 9) / 2;
    }
    return base;
}

qint32 CLinePreparer::base2Bits(qint32 base)
{
    qint32 bits = 2;
    if (base < 10)
    {
        return bits + base;
    }
    else
    {
        return bits + (2 * base) - 9;
    }
}
