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

#include "headers/IHdrSubfile.h"

class CRgn2;
class CRgn3;
class CRgn4;
class CSection;

class CHdrRgn : public IHdrSubfile
{
public:
    CHdrRgn();
    virtual ~CHdrRgn() = default;

    void write(QFile& file) override;

    void setRgn1(CSection& section);
    void setRgn2(CRgn2& section);
    void setRgn3(CRgn3& section);
    void setRgn4(CRgn4& section);
    void setRgn5(CSection& section);

protected:
#pragma pack(1)
    struct data_t
    {
        quint32 offset1 = 0;            ///< 0x00000015 .. 0x00000018 rgn1
        quint32 length1 = 0;            ///< 0x00000019 .. 0x0000001C
        quint32 offset_polyg2 = 0;      ///< 0x0000001D .. 0x00000020 rgn2
        quint32 length_polyg2 = 0;      ///< 0x00000021 .. 0x00000024
        quint8 byte0x00000025_0x00000038[20] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x20, 0xFD, 0xFC, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
        quint32 offset_polyl2 = 0;      ///< 0x00000039 .. 0x0000003C rgn3
        quint32 length_polyl2 = 0;      ///< 0x0000003D .. 0x00000040
        quint8 byte0x00000041_0x00000054[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x20, 0xFD, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        quint32 offset_point2 = 0;      ///< 0x00000055 .. 0x00000058 rgn4
        quint32 length_point2 = 0;      ///< 0x00000059 .. 0x0000005C
        quint8 byte0x0000005D_0x00000070[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x07, 0x00, 0x20, 0x3F, 0xF7, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00};
        quint32 offset2 = 0;            ///< 0x00000071 .. 0x00000074 rgn5
        quint32 length2 = 0;            ///< 0x00000075 .. 0x00000078
        quint32 unknown = 0;            ///< 0x00000079 .. 0x0000007C
    };
#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
    data_t data;
};


