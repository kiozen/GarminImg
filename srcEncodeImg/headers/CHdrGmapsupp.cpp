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

#include "CException.h"
#include "headers/CHdrGmapsupp.h"

#include <tuple>

CHdrGmapsupp::CHdrGmapsupp()
{
    const QDateTime& timestamp = QDateTime::currentDateTimeUtc();
    const QDate& date = timestamp.date();
    const QTime& time = timestamp.time();

    data_.upMonth = date.month();
    data_.upYear = date.year() - 1900;

    data_.year = date.year();
    data_.month = date.month();
    data_.day = date.day();
    data_.hour = time.hour();
    data_.minute = time.minute();
    data_.second = time.second();

    memset(data_.desc1, 0x20, sizeof(data_.desc1));
    memset(data_.desc2, 0x20, sizeof(data_.desc2));
}

quint32 CHdrGmapsupp::blocksize() const
{
    return data_.blocksize();
}

void CHdrGmapsupp::setDescription(const QString& str)
{
    if(str.size() > 50)
    {
        throw CException("Description must not exceed 50 characters.");
    }

    memcpy(data_.desc1, str.toLatin1().data(), qMin(qint32(sizeof(data_.desc1)), str.size()));
    if(str.size() > 20)
    {
        memcpy(data_.desc2, str.mid(20).toLatin1().data(), qMin(qint32(sizeof(data_.desc2)), str.size() - 20));
    }
}

void CHdrGmapsupp::setBlockcount(quint32 count)
{
    numBlocks = count;
}

quint32 CHdrGmapsupp::hash() const
{
    QCryptographicHash hash(QCryptographicHash::Md4);
    hash.addData((char*)&data_, sizeof(data_));
    const QByteArray& result = hash.result();
    const quint32 * p = (const quint32 *)result.data();
    return p[0] ^ p[1] ^ p[2] ^ p[3];
}

void CHdrGmapsupp::write(QFile& file)
{
    writeSizeValues(data_.blocksize());

    file.seek(0);
    file.write((const char*)&data_,  sizeof(data_));
    file.seek(offsetFAT());
}


// Try out various values of h, s and c until we find a combination that is large enough.
// I'm not entirely sure about the valid values, but it seems that only certain values work
// which is why we use values from a list.
// See: http://www.win.tue.nl/~aeb/partitions/partition_types-2.html for justification for the h list
std::tuple<qint32, qint32, qint32> findValues(qint32 endSector)
{
    for (qint32 h : {16, 32, 64, 128, 256})
    {
        for (qint32 s : {4, 8, 16, 32})
        {
            for (qint32 c : {0x20, 0x40, 0x80, 0x100, 0x200, 0x3ff})
            {
                if (s * h * c > endSector)
                {
                    return {h, s, c};
                }
            }
        }
    }

    return {128, 32, 0x400};
}

/**
 * Write out the values associated with the partition sizes.
 *
 * @param blockSize Block size.
 */
void CHdrGmapsupp::writeSizeValues(qint32 blockSize)
{
    qint32 endSector = (qint32) (((numBlocks + 1L) * blockSize + 511) / 512);
    //System.out.printf("end sector %d %x\n", endSector, endSector);

    auto [headsPerCylinder, sectorsPerTrack, cyls] = findValues(endSector);

    // This sectors, head, cylinders stuff appears to be used by mapsource
    // and they have to be larger than the actual size of the map.  It
    // doesn't appear to have any effect on a garmin device or other software.
    data_.sectors1 = sectorsPerTrack;
    data_.sectors2 = sectorsPerTrack;
    data_.heads1 = headsPerCylinder;
    data_.heads2 = headsPerCylinder;
    data_.cylinders = cyls;

    // Since there are only 2 bytes here it can overflow, if it
    // does we replace it with 0xffff.
    qint32 blocks = (qint32) (endSector * 512L / blockSize);
    data_.nBlocks1 = qMin(blocks, 0x0ffff);

    qint32 lba = endSector - 1;
    qint32 h = (lba / sectorsPerTrack) % headsPerCylinder;
    qint32 s = (lba % sectorsPerTrack) + 1;
    qint32 c = lba / (sectorsPerTrack * headsPerCylinder);

    data_.endHead = h;
    data_.endSector = ((s) | ((c >> 2) & 0xc0));
    data_.endCylinder = c & 0x0FF;

    data_.nSectors = endSector;
}
