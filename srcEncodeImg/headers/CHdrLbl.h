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

class CSection;
class CLbl1;
class CLbl12;
class CLbl28;
class CLbl29;

class CHdrLbl : public IHdrSubfile
{
public:
    CHdrLbl();
    virtual ~CHdrLbl() = default;

    void write(QFile& file) override;
    void setLbl1(CLbl1 &section);
    void setLbl2(CSection& section);
    void setLbl3(CSection& section);
    void setLbl4(CSection& section);
    void setLbl5(CSection& section);
    void setLbl6(CSection& section);
    void setLbl7(CSection& section);
    void setLbl8(CSection& section);
    void setLbl9(CSection& section);
    void setLbl10(CSection& section);

    void setLbl11(CSection& section);
    void setLbl12(CLbl12 &section);
    void setLbl13(CSection& section);
    void setLbl14(CSection& section);
    void setLbl15(CSection& section);
    void setLbl16(CSection& section);
    void setLbl17(CSection& section);
    void setLbl18(CSection& section);
    void setLbl19(CSection& section);
    void setLbl20(CSection& section);

    void setLbl21(CSection& section);
    void setLbl22(CSection& section);
    void setLbl23(CSection& section);
    void setLbl24(CSection& section);
    void setLbl25(CSection& section);
    void setLbl26(CSection& section);
    void setLbl27(CSection& section);
    void setLbl28(CLbl28 &section);
    void setLbl29(CLbl29& section);
    void setLbl30(CSection& section);

    void setLbl31(CSection& section);
    void setLbl32(CSection& section);
    void setLbl33(CSection& section);
    void setLbl34(CSection& section);
    void setLbl35(CSection& section);
    void setLbl36(CSection& section);
    void setLbl37(CSection& section);

protected:
#pragma pack(1)
    struct data_t
    {
        quint32 lbl1_offset = 0;        ///< 0x00000015 .. 0x00000018   // writer.put4(HEADER_LEN + sortDescriptionLength);
        quint32 lbl1_length = 0;        ///< 0x00000019 .. 0x0000001C   // writer.put4(getLabelSize());
        quint8 addr_shift = 0;          ///< 0x0000001D always 0        // writer.put1u(offsetMultiplier);
        quint8 coding = 0x09;           ///< 0x0000001E                 // writer.put1u(encodingType);
        quint32 lbl2_offset = 0;        ///< 0x0000001F .. 0x00000022   // not used
        quint32 lbl2_length = 0;        ///< 0x00000023 .. 0x00000026
        quint16 lbl2_rec_size = 0;      ///< 0x00000027 .. 0x00000028
        quint8 byte0x00000029_0x0000002C[4] = {0};
        quint32 lbl3_offset = 0;        ///< 0x0000002D .. 0x00000030   // not used
        quint32 lbl3_length = 0;        ///< 0x00000031 .. 0x00000034
        quint16 lbl3_rec_size = 0;      ///< 0x00000035 .. 0x00000036
        quint8 byte0x00000037_0x0000003A[4] = {0};
        quint32 lbl4_offset = 0;        ///< 0x0000003B .. 0x0000003E   // not used
        quint32 lbl4_length = 0;        ///< 0x0000003F .. 0x00000042
        quint16 lbl4_rec_size = 0;      ///< 0x00000043 .. 0x00000044
        quint8 byte0x00000045_0x00000048[4] = {0};
        quint32 lbl5_offset = 0;        ///< 0x00000049 .. 0x0000004C   // not used
        quint32 lbl5_length = 0;        ///< 0x0000004D .. 0x00000050
        quint16 lbl5_rec_size = 0;      ///< 0x00000051 .. 0x00000052
        quint8 byte0x00000053_0x00000056[4] = {0};
        quint32 lbl6_offset = 0;        ///< 0x00000057 .. 0x0000005A   // not used
        quint32 lbl6_length = 0;        ///< 0x0000005B .. 0x0000005E
        quint8 lbl6_addr_shift = 0;     ///< 0x0000005F
        quint8 lbl6_glob_mask = 0;      ///< 0x00000060
        quint8 byte0x00000061_0x00000063[3] = {0};
        quint32 lbl7_offset = 0;        ///< 0x00000064 .. 0x00000067   // not used
        quint32 lbl7_length = 0;        ///< 0x00000068 .. 0x0000006B
        quint16 lbl7_rec_size = 0;      ///< 0x0000006C .. 0x0000006D
        quint8 byte0x0000006E_0x00000071[4] = {0};
        quint32 lbl8_offset = 0;        ///< 0x00000072 .. 0x00000075   // not used
        quint32 lbl8_length = 0;        ///< 0x00000076 .. 0x00000079
        quint16 lbl8_rec_size = 0;      ///< 0x0000007A .. 0x0000007B
        quint8 byte0x0000007C_0x0000007F[4] = {0};
        quint32 lbl9_offset = 0;        ///< 0x00000080 .. 0x00000083   // not used
        quint32 lbl9_length = 0;        ///< 0x00000084 .. 0x00000087
        quint16 lbl9_rec_size = 0;      ///< 0x00000088 .. 0x00000089
        quint8 byte0x0000008A_0x0000008D[4] = {0};
        quint32 lbl10_offset = 0;       ///< 0x0000008E .. 0x00000091   // not used
        quint32 lbl10_length = 0;       ///< 0x00000092 .. 0x00000095
        quint16 lbl10_rec_size = 0;     ///< 0x00000096 .. 0x00000097
        quint8 byte0x00000098_0x0000009B[4] = {0};
        quint32 lbl11_offset = 0;       ///< 0x0000009C .. 0x0000009F   // not used
        quint32 lbl11_length = 0;       ///< 0x000000A0 .. 0x000000A3
        quint16 lbl11_rec_size = 0;     ///< 0x000000A4 .. 0x000000A5
        quint8 byte0x000000A6_0x000000A9[4] = {0};
        quint16 codepage = 1252;        ///< 0x000000AA .. 0x000000AB  //
        quint8 byte0x000000AC_0x000000AF[4] = {0x00, 0x00, 0x01, 0x00}; // 07 00 02 80 or 12 00 01 80
        quint32 lbl12_offset = 0;       ///< 0x000000B0 .. 0x000000B3
        quint32 lbl12_length = 0;       ///< 0x000000B4 .. 0x000000B7
        quint32 lbl13_offset = 0;       ///< 0x000000B8 .. 0x000000BB
        quint32 lbl13_length = 0;       ///< 0x000000BC .. 0x000000BF
        quint16 lbl13_rec_size = 0;     ///< 0x000000C0 .. 0x000000C1
        quint8 byte0x000000C2_0x000000C3[2] = {0};
        quint32 lbl14_offset = 0;       ///< 0x000000C4 .. 0x000000C7
        quint32 lbl14_length = 0;       ///< 0x000000C8 .. 0x000000CB
        quint16 lbl14_rec_size = 0;     ///< 0x000000CC .. 0x000000CD
        quint8 byte0x000000CE_0x000000CF[2] = {0};
        quint32 lbl15_offset = 0;       ///< 0x000000D0 .. 0x000000D3
        quint32 lbl15_length = 0;       ///< 0x000000D4 .. 0x000000D7
        quint16 lbl15_rec_size = 0;     ///< 0x000000D8 .. 0x000000D9
        quint8 byte0x000000DA_0x000000DD[4] = {0};
        quint32 lbl16_offset = 0;       ///< 0x000000DE .. 0x000000E1
        quint32 lbl16_length = 0;       ///< 0x000000E2 .. 0x000000E5
        quint16 lbl16_rec_size = 0;     ///< 0x000000E6 .. 0x000000E7
        quint8 byte0x000000E8_0x000000EB[4] = {0};
        quint32 lbl17_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl17_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl17_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000000F6_0x000000F9[4] = {0};
        quint32 lbl18_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl18_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl18_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x00000104_0x00000107[4] = {0};
        quint32 lbl19_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl19_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl19_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x00000112_0x00000115[4] = {0};
        quint32 lbl20_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl20_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl20_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x00000120_0x00000123[4] = {0};
        quint32 lbl21_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl21_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl21_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x0000012E_0x00000131[4] = {0};
        quint32 lbl22_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl22_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl22_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x0000013C_0x0000013F[4] = {0};
        quint32 lbl23_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl23_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl23_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x0000014A_0x0000014D[4] = {0};
        quint32 lbl24_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl24_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl24_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x0000015A_0x0000015B[2] = {0};
        quint32 lbl25_offset;           ///< 0x000000EC .. 0x000000EF
        quint32 lbl25_length;           ///< 0x000000F0 .. 0x000000F3
        quint16 lbl25_rec_size;         ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x00000165_0x00000168[4] = {0};
        quint32 lbl26_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl26_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl26_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x00000172_0x00000175[4] = {0};
        quint32 lbl27_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl27_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl27_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x00000180_0x00000183[4] = {0};
        quint32 lbl28_offset = 0;       ///< 0x000000EC .. 0x000000EF   // list of JPG offsets into LBL29
        quint32 lbl28_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl28_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x0000018E_0x00000191[4] = {0};
        quint32 lbl29_offset = 0;       ///< 0x000000EC .. 0x000000EF   // all JPGs
        quint32 lbl29_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint32 lbl30_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl30_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl30_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001A4_0x000001A5[2] = {0};
        quint32 lbl31_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl31_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl31_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001B0_0x000001B1[2] = {0};
        quint32 lbl32_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl32_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl32_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001BC_0x000001BD[2] = {0};
        quint32 lbl33_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl33_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl33_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001C8_0x000001C9[2] = {0};
        quint32 lbl34_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl34_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl34_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001D4_0x000001D7[4] = {0};
        quint32 lbl35_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl35_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl35_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001E2_0x000001E5[4] = {0};
        quint32 lbl36_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl36_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl36_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001F0_0x000001F1[2] = {0};
        quint32 lbl37_offset = 0;       ///< 0x000000EC .. 0x000000EF
        quint32 lbl37_length = 0;       ///< 0x000000F0 .. 0x000000F3
        quint16 lbl37_rec_size = 0;     ///< 0x000000F4 .. 0x000000F5
        quint8 byte0x000001FC_0x000001FF[4] = {0};
    };
#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
    data_t data;
};


