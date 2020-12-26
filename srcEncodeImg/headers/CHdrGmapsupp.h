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

#include <QtCore>

class CHdrGmapsupp
{
public:
    CHdrGmapsupp();
    virtual ~CHdrGmapsupp() = default;

    quint32 blocksize() const;
    quint32 offsetFAT() const {return data_.offsetFAT * 0x200;}
    void setDescription(const QString& str);
    void setBlockcount(quint32 count);

    void write(QFile& file);

    quint32 hash() const;

protected:
    void writeSizeValues(int blockSize);
#pragma pack(1)
    struct data_t
    {
        quint8 xorByte = 0;                 ///< 0x00000000
        quint8 byte0x00000001_0x00000007[7] = {0};
        quint16 version = 2;                ///< 0x00000008
        quint8 upMonth = 0;                 ///< 0x0000000A
        quint8 upYear = 0;                  ///< 0x0000000B
        quint8 byte0x0000000C_0x0000000D[2] = {0};
        quint8 supp = 0;                    ///< 0x0000000E
        quint8 checksum = 0;                ///< 0x0000000F
        char signature[7] = {0x44, 0x53, 0x4B, 0x49, 0x4D, 0x47, 0x00}; ///< 0x00000010 .. 0x00000016
        quint8 byte0x00000017 = 0x2;
        quint16 sectors1 = 0;               ///< 0x00000018 .. 0x00000019
        quint16 heads1 = 0;                 ///< 0x0000001A .. 0x0000001B
        quint16 cylinders = 0;              ///< 0x0000001C .. 0x0000001D
        quint8 byte0x0000001E_0x000000038[27] = {0};
        qint16 year = 0;                    ///< 0x00000039 .. 0x0000003A
        qint8 month = 0;                    ///< 0x0000003B
        qint8 day = 0;                      ///< 0x0000003C
        qint8 hour = 0;                     ///< 0x0000003D
        qint8 minute = 0;                   ///< 0x0000003E
        qint8 second = 0;                   ///< 0x0000003F
        qint8 offsetFAT = 2;                ///< 0x00000040 offset of the FAT in multiple of 0x200
        char identifier[7] = {0x47, 0x41, 0x52, 0x4D, 0x49, 0x4E, 0x00}; ///< 0x00000041 .. 0x00000047
        quint8 byte0x00000048 = 0;
        char desc1[20] = {0x20};            ///< 0x00000049 .. 0x0000005C
        quint16 heads2 = 0;                 ///< 0x0000005D .. 0x0000005E
        quint16 sectors2 = 0;               ///< 0x0000005F .. 0x00000060
        quint8 e1 = 9;                      ///< 0x00000061
        quint8 e2 = 7;                      ///< 0x00000062
        quint16 nBlocks1 = 0;               ///< 0x00000063 .. 0x00000064
        char desc2[30] = {0x20};            ///< 0x00000065 .. 0x00000082
        quint8 byte0x00000083_0x000001BE[0x13C] = {0};
        quint8 startHead = 0;               ///< 0x000001BF
        quint8 startSector = 1;             ///< 0x000001C0
        quint8 startCylinder = 0;           ///< 0x000001C1
        quint8 systemType = 0;              ///< 0x000001C2
        quint8 endHead = 0;                 ///< 0x000001C3
        quint8 endSector = 0;               ///< 0x000001C4
        quint8 endCylinder = 0;             ///< 0x000001C5
        quint32 relSectors = 0;              ///< 0x000001C6..0x000001C9
        quint32 nSectors = 0;               ///< 0x000001CA .. 0x000001CD
        quint8 byte0x0000001CE_0x000001FD[0x30] = {0};
        quint16 terminator = 0xAA55;        ///< 0x000001FE .. 0x000001FF
        quint8 byte0x000000200_0x00000FFF[0xE00] = {0};

        quint32 blocksize() const {return 1 << (e1 + e2);}
    };
#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif

    data_t data_;
    int numBlocks = 0;
};


