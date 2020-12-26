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

#include <QByteArray>

class CBitWriter : public QByteArray
{
public:
    CBitWriter();
    virtual ~CBitWriter() = default;

    qint32 getBitPosition() const {return bitoff;}
    /**
     * Put exactly one bit into the buffer.
     *
     * @param b The bottom bit of the integer is set at the current bit position.
     */
    void put1(qint32 b);
    void put1(bool b);
    /**
     * Put a number of bits into the buffer, growing it if necessary.
     *
     * @param bval The bits to add, the lowest <b>n</b> bits will be added to
     * the buffer.
     * @param nb The number of bits.
     */
    void putn(quint32 bval, qint32 nb);
    /**
     * Write a signed value. If the value doesn't fit into nb bits, write one or more 1 << (nb-1)
     * as a flag for extended range.
     */
    void sputn(qint32 bval, qint32 nb);

    CBitWriter& operator=(const CBitWriter& o)
    {
        QByteArray::operator=(o);
        bitoff = o.bitoff;
    }

private:
    /**
     * Set everything up so that the given size can be accommodated.
     * The buffer is re-sized if necessary.
     *
     * @param newlen The new length of the bit buffer in bits.
     */
    void ensureSize(qint32 newlen);
    /**
     * Get the byte offset for the given bit number.
     *
     * @param boff The number of the bit in question.
     * @return The index into the byte array where the bit resides.
     */
    qint32 getByteOffset(qint32 boff);

    // Choose so that most roads will not fill it.
    static constexpr auto INITIAL_BUF_SIZE = 20;
    static constexpr auto BUFSIZE_INC = 50;

    // The bit offset into the byte array.
    qint32 bitoff = 0;
};


