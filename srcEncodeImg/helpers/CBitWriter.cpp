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

#include "helpers/CBitWriter.h"

#include "CException.h"

CBitWriter::CBitWriter() {}

void CBitWriter::ensureSize(qint32 newlen) {
  qint32 s1 = size();
  if (newlen / 8 >= s1) {
    qint32 s2 = (newlen / 8) + 1;
    for (; s1 < s2; s1++) {
      append('\0');
    }
  }
}

/**
 * Get the byte offset for the given bit number.
 *
 * @param boff The number of the bit in question.
 * @return The index into the byte array where the bit resides.
 */
qint32 CBitWriter::getByteOffset(qint32 boff) { return boff / 8; }

/**
 * Put exactly one bit into the buffer.
 *
 * @param b The bottom bit of the integer is set at the current bit position.
 */
void CBitWriter::put1(qint32 b) {
  ensureSize(bitoff + 1);

  qint32 off = getByteOffset(bitoff);

  // Get the remaining bits into the byte.
  qint32 rem = bitoff - 8 * off;

  // Or it in, we are assuming that the position is never turned back.
  data()[off] |= (b & 0x1) << rem;

  // Increment position
  bitoff++;
}

void CBitWriter::put1(bool b) { put1(b ? 1 : 0); }

/**
 * Put a number of bits into the buffer, growing it if necessary.
 *
 * @param bval The bits to add, the lowest <b>n</b> bits will be added to
 * the buffer.
 * @param nb The number of bits.
 */
void CBitWriter::putn(quint32 bval, qint32 nb) {
  quint32 val = bval & ((1 << nb) - 1);
  qint32 n = nb;

  // We need to be able to deal with more than 24 bits, but now we can't yet
  if (n >= 24) {
    throw CException("We need to be able to deal with more than 24 bits, but now we can't yet");
  }

  ensureSize(bitoff + n);

  // Get each affected byte and set bits into it until we are done.
  while (n > 0) {
    qint32 ind = getByteOffset(bitoff);
    qint32 rem = bitoff - 8 * ind;

    data()[ind] |= ((val << rem) & 0xff);

    // Shift down in preparation for next byte.
    val >>= 8 - rem;

    // Account for change so far
    qint32 nput = 8 - rem;
    if (nput > n) {
      nput = n;
    }
    bitoff += nput;
    n -= nput;
  }
}

/**
 * Write a signed value. If the value doesn't fit into nb bits, write one or more 1 << (nb-1)
 * as a flag for extended range.
 */
void CBitWriter::sputn(qint32 bval, qint32 nb) {
  qint32 top = 1 << (nb - 1);
  qint32 mask = top - 1;
  qint32 val = qAbs(bval);
  while (val > mask) {
    putn(top, nb);
    val -= mask;
  }
  if (bval < 0) {
    putn((top - val) | top, nb);
  } else {
    putn(val, nb);
  }
}
