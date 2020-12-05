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

**********************************************************************************************/

#pragma once

#include <proj_api.h>
#include <QImage>
#include <QtCore>
#ifdef __MINGW32__
#undef LP
#endif

class IGarminStrTbl;

#define GARMIN_DEG(x) ((x) < 0x800000 ? (double)(x) * 360.0 / 16777216.0 : (double)((x) - 0x1000000) * 360.0 / 16777216.0)
#define GARMIN_RAD(x) ((x) < 0x800000 ? (double)(x) * (2 * M_PI) / 16777216.0 : (double)((x) - 0x1000000) * (2 * M_PI) / 16777216.0)
typedef quint8 quint24[3];


#pragma pack(1)
struct gmapsupp_imghdr_t
{
    quint8 xorByte = 0;                 ///< 0x00000000
    quint8 byte0x00000001_0x00000007[7] = {0};
    quint16 version = 0;                ///< 0x00000008
    quint8 upMonth = 0;                 ///< 0x0000000A
    quint8 upYear = 0;                  ///< 0x0000000B
    quint8 byte0x0000000C_0x0000000D[2] = {0};
    quint8 supp = 0;                    ///< 0x0000000E
    quint8 checksum = 0;                ///< 0x0000000F
    char signature[7] = {0};            ///< 0x00000010 .. 0x00000016
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
    qint8 offsetFAT = 0;                ///< 0x00000040
    char identifier[7];                 ///< 0x00000041 .. 0x00000047
    quint8 byte0x00000048;
    char desc1[20];                     ///< 0x00000049 .. 0x0000005C
    quint16 head2 = 0;                  ///< 0x0000005D .. 0x0000005E
    quint16 sectors2 = 0;               ///< 0x0000005F .. 0x00000060
    quint8 e1 = 0;                      ///< 0x00000061
    quint8 e2 = 0;                      ///< 0x00000062
    quint16 nBlocks1;                   ///< 0x00000063 .. 0x00000064
    char desc2[30];                     ///< 0x00000065 .. 0x00000082
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

    quint32 blocksize(){return 1 << (e1 + e2);}
    void print();
};

struct FATBlock_t
{
    quint8 flag;                 ///< 0x00000000
    char name[8];                ///< 0x00000001 .. 0x00000008
    char type[3];                ///< 0x00000009 .. 0x0000000B
    quint32 size;                ///< 0x0000000C .. 0x0000000F
    quint16 part;                ///< 0x00000010 .. 0x00000011
    quint8 byte0x00000012_0x0000001F[14];
    quint16 blocks[240];         ///< 0x00000020 .. 0x000001FF
};

struct subfile_hdr_t
{
    quint16 size;
    char type[10];               ///< 0x00000002 .. 0x0000000B
    quint8 byte0x0000000C;
    quint8 flag;                 ///< 0x0000000D
    qint16 year;
    qint8 month;
    qint8 day;
    qint8 hour;
    qint8 minute;
    qint8 second;

    void print();
};

struct gmp_hdr_t : public subfile_hdr_t
{
    quint8 byte0x00000015_0x00000018[4];
    quint32 offsetTRE;
    quint32 offsetRGN;
    quint32 offsetLBL;
    quint32 offsetNET;
    quint32 offsetNOD;
    quint32 offsetDEM;
    quint8 byte0x00000031_0x00000034[4];

    void print();
};


struct hdr_tre_t : public subfile_hdr_t
{
    quint24 northbound = {0};        ///< 0x00000015 .. 0x00000017  // writer.put3s(area.getMaxLat());
    quint24 eastbound = {0};         ///< 0x00000018 .. 0x0000001A  // Utils.put3sLongitude(writer, area.getMaxLong());
    quint24 southbound = {0};        ///< 0x0000001B .. 0x0000001D  // writer.put3s(area.getMinLat());
    quint24 westbound = {0};         ///< 0x0000001E .. 0x00000020  // writer.put3s(area.getMinLong());  // minLong cant be +180
    quint32 tre1_offset = 0;         ///< 0x00000021 .. 0x00000024  // writer.put4(getMapLevelsPos());
    quint32 tre1_size = 0;           ///< 0x00000025 .. 0x00000028  // writer.put4(getMapLevelsSize());
    quint32 tre2_offset = 0;         ///< 0x00000029 .. 0x0000002C  // writer.put4(getSubdivPos());
    quint32 tre2_size = 0;           ///< 0x0000002D .. 0x00000030  // writer.put4(getSubdivSize());
    quint32 tre3_offset = 0;         ///< 0x00000031 .. 0x00000034  // copyright.writeSectionInfo(writer);
    quint32 tre3_size = 0;           ///< 0x00000035 .. 0x00000038  //
    quint16 tre3_rec_size = 0;       ///< 0x00000039 .. 0x0000003A  //
    quint8 byte0x0000003B_0x0000003E[4] = {0};                      // writer.put4(0);
    quint8 POI_flags = 0;            ///< 0x0000003F                // writer.put1u(getPoiDisplayFlags());
    quint24 render_prio = {0x14, 0, 0};///< 0x00000040 .. 0x00000042  // 0x0014
    quint8 byte0x00000043_0x00000049[7] = {0x01, 0x03, 0x11, 0x00, 0x01, 0x00, 0x00};                  // 00 01 08 24 00 01 00 00 to be investigated
    quint32 tre4_offset = 0;         ///< 0x0000004A .. 0x0000004D  // polyline.writeSectionInfo(writer);
    quint32 tre4_size = 0;           ///< 0x0000004E .. 0x00000051  //
    quint16 tre4_rec_size = 0;       ///< 0x00000052 .. 0x00000053  //
    quint8 byte0x00000054_0x00000057[4] = {0};                      // writer.put4(0);
    quint32 tre5_offset = 0;         ///< 0x00000058 .. 0x0000005B  // polygon.writeSectionInfo(writer);
    quint32 tre5_size = 0;           ///< 0x0000005C .. 0x0000005F  //
    quint16 tre5_rec_size = 0;       ///< 0x00000060 .. 0x00000061  //
    quint8 byte0x00000062_0x00000065[4] = {0};                      // writer.put4(0);
    quint32 tre6_offset = 0;         ///< 0x00000066 .. 0x00000069  // points.writeSectionInfo(writer);
    quint32 tre6_size = 0;           ///< 0x0000006A .. 0x0000006D  //
    quint16 tre6_rec_size = 0;       ///< 0x0000006E .. 0x0000006F  //
    quint8 byte0x00000070_0x00000073[4] = {0};                      // writer.put4(0);
    /*-----------------------------------------------------*/
    quint32 map_id = 0;              ///< 0x00000074 .. 0x00000077  // writer.put4(getMapId());
    quint8 byte0x00000078_0x0000007B[4] = {0};                      // writer.put4(0);
    // Object groups V2 (CTreGroup2).
    quint32 tre7_offset = 0;         ///< 0x0000007C .. 0x0000007F  //Groups2Offset     // extTypeOffsets.writeSectionInfo(writer, true);
    quint32 tre7_size = 0;           ///< 0x00000080 .. 0x00000083  //dwGroups2Length
    quint16 tre7_rec_size = 0;       ///< 0x00000084 .. 0x00000085  //wGroup2RecSize
    quint8 byte0x00000086_0x00000089[4] = {0};                      // 01 00 00 00
    // Order: polyline, polygon, POI; each sorted by type (1 type 1 levels 1 subtype)
    quint32 tre8_offset = 0;         ///< 0x0000008A .. 0x0000008D  // extTypeOverviews.writeSectionInfo(writer);
    quint32 tre8_size = 0;           ///< 0x0000008E .. 0x00000091  //
    quint16 tre8_rec_size = 0;       ///< 0x00000092 .. 0x00000093  //
    quint16 polyl2_types_num = 0;    ///< 0x00000094 .. 0x00000095  // writer.put2u(numExtTypeLineTypes);
    quint16 polyg2_types_num = 0;    ///< 0x00000096 .. 0x00000097  // writer.put2u(numExtTypeAreaTypes);
    quint16 poi2_types_num = 0;      ///< 0x00000098 .. 0x00000099  // writer.put2u(numExtTypePointTypes);

    /*-----------------------------------------------------*/
    quint8 key[16] = {0};            ///< 0x0000009A .. 0x000000A5  // MapValues mv = new MapValues(mapId, getHeaderLength());
    // mv.calculate();
    // writer.put4(mv.value(0));
    // writer.put4(mv.value(1));
    // writer.put4(mv.value(2));
    // writer.put4(mv.value(3));

    quint8 byte0x000000AA_0x000000AD[4] = {0};                  // writer.put4(0);
    quint32 tre9_offset;         ///< 0x000000AE .. 0x000000B1  // writer.put4(0);
    quint32 tre9_size;           ///< 0x000000B2 .. 0x000000B5  // writer.put4(0);
    quint16 tre9_rec_size;       ///< 0x000000B6 .. 0x000000B7  // writer.put2u(0);
    quint8 byte0x000000B8_0x000000BB[4] = {0};                  // writer.put4(0);
    quint32 tre10_offset;         ///< 0x000000BC .. 0x000000BF //
    quint32 tre10_size;           ///< 0x000000C0 .. 0x000000C3 //
    quint16 tre10_rec_size;       ///< 0x000000C4 .. 0x000000C5 //
    quint8 byte0x000000C6_0x000000CE[9] = {0};                  //
    quint32 map_number;           ///< 0x00000CF .. 0x000000D2  // map number as hex

    void print(quint32 offset);
};

struct hdr_rgn_t : public subfile_hdr_t
{
    quint32 offset1 = 0;                ///< 0x00000015 .. 0x00000018
    quint32 length1 = 0;                ///< 0x00000019 .. 0x0000001C
    quint32 offset_polyg2 = 0;          ///< 0x0000001D .. 0x00000020
    quint32 length_polyg2 = 0;          ///< 0x00000021 .. 0x00000024
    quint8 byte0x00000025_0x00000038[20] = {0};
    quint32 offset_polyl2 = 0;          ///< 0x00000039 .. 0x0000003C
    quint32 length_polyl2 = 0;          ///< 0x0000003D .. 0x00000040
    quint8 byte0x00000041_0x00000054[20] = {0};
    quint32 offset_point2 = 0;          ///< 0x00000055 .. 0x00000058
    quint32 length_point2 = 0;          ///< 0x00000059 .. 0x0000005C
    quint8 byte0x0000005D_0x00000070[20] = {0};
    quint32 offset2 = 0;                ///< 0x00000071 .. 0x00000074
    quint32 length2 = 0;                ///< 0x00000075 .. 0x00000078
    quint32 unknown = 0;                ///< 0x00000079 .. 0x0000007C
    void print(quint32 offset);
};

struct hdr_lbl_t : public subfile_hdr_t
{
    quint32 lbl1_offset = 0;            ///< 0x00000015 .. 0x00000018   // writer.put4(HEADER_LEN + sortDescriptionLength);
    quint32 lbl1_length = 0;            ///< 0x00000019 .. 0x0000001C   // writer.put4(getLabelSize());
    quint8 addr_shift = 0;              ///< 0x0000001D                 // writer.put1u(offsetMultiplier);
    quint8 coding = 0;                  ///< 0x0000001E                 // writer.put1u(encodingType);
    quint32 lbl2_offset = 0;            ///< 0x0000001F .. 0x00000022   // not used
    quint32 lbl2_length = 0;            ///< 0x00000023 .. 0x00000026
    quint16 lbl2_rec_size = 0;          ///< 0x00000027 .. 0x00000028
    quint8 byte0x00000029_0x0000002C[4] = {0};
    quint32 lbl3_offset = 0;            ///< 0x0000002D .. 0x00000030   // not used
    quint32 lbl3_length = 0;            ///< 0x00000031 .. 0x00000034
    quint16 lbl3_rec_size = 0;          ///< 0x00000035 .. 0x00000036
    quint8 byte0x00000037_0x0000003A[4] = {0};
    quint32 lbl4_offset = 0;            ///< 0x0000003B .. 0x0000003E   // not used
    quint32 lbl4_length = 0;            ///< 0x0000003F .. 0x00000042
    quint16 lbl4_rec_size = 0;          ///< 0x00000043 .. 0x00000044
    quint8 byte0x00000045_0x00000048[4] = {0};
    quint32 lbl5_offset = 0;            ///< 0x00000049 .. 0x0000004C   // not used
    quint32 lbl5_length = 0;            ///< 0x0000004D .. 0x00000050
    quint16 lbl5_rec_size = 0;          ///< 0x00000051 .. 0x00000052
    quint8 byte0x00000053_0x00000056[4] = {0};
    quint32 lbl6_offset = 0;            ///< 0x00000057 .. 0x0000005A   // not used
    quint32 lbl6_length = 0;            ///< 0x0000005B .. 0x0000005E
    quint8 lbl6_addr_shift = 0;         ///< 0x0000005F
    quint8 lbl6_glob_mask = 0;          ///< 0x00000060
    quint8 byte0x00000061_0x00000063[3] = {0};
    quint32 lbl7_offset = 0;            ///< 0x00000064 .. 0x00000067   // not used
    quint32 lbl7_length = 0;            ///< 0x00000068 .. 0x0000006B
    quint16 lbl7_rec_size = 0;          ///< 0x0000006C .. 0x0000006D
    quint8 byte0x0000006E_0x00000071[4] = {0};
    quint32 lbl8_offset = 0;            ///< 0x00000072 .. 0x00000075   // not used
    quint32 lbl8_length = 0;            ///< 0x00000076 .. 0x00000079
    quint16 lbl8_rec_size = 0;          ///< 0x0000007A .. 0x0000007B
    quint8 byte0x0000007C_0x0000007F[4] = {0};
    quint32 lbl9_offset = 0;            ///< 0x00000080 .. 0x00000083   // not used
    quint32 lbl9_length = 0;            ///< 0x00000084 .. 0x00000087
    quint16 lbl9_rec_size = 0;          ///< 0x00000088 .. 0x00000089
    quint8 byte0x0000008A_0x0000008D[4] = {0};
    quint32 lbl10_offset = 0;           ///< 0x0000008E .. 0x00000091   // not used
    quint32 lbl10_length = 0;           ///< 0x00000092 .. 0x00000095
    quint16 lbl10_rec_size = 0;         ///< 0x00000096 .. 0x00000097
    quint8 byte0x00000098_0x0000009B[4] = {0};
    quint32 lbl11_offset = 0;           ///< 0x0000009C .. 0x0000009F   // not used
    quint32 lbl11_length = 0;           ///< 0x000000A0 .. 0x000000A3
    quint16 lbl11_rec_size = 0;         ///< 0x000000A4 .. 0x000000A5
    quint8 byte0x000000A6_0x000000A9[4] = {0};
    quint16 codepage = 0;               ///< 0x000000AA .. 0x000000AB  optional check length
    quint8 byte0x000000AC_0x000000AF[4] = {0};  // 07 00 02 80 or 12 00 01 80
    quint32 lbl12_offset = 0;           ///< 0x000000B0 .. 0x000000B3
    quint32 lbl12_length = 0;           ///< 0x000000B4 .. 0x000000B7
    quint32 lbl13_offset = 0;           ///< 0x000000B8 .. 0x000000BB
    quint32 lbl13_length = 0;           ///< 0x000000BC .. 0x000000BF
    quint16 lbl13_rec_size = 0;         ///< 0x000000C0 .. 0x000000C1
    quint8 byte0x000000C2_0x000000C3[2] = {0};
    quint32 lbl14_offset = 0;           ///< 0x000000C4 .. 0x000000C7
    quint32 lbl14_length = 0;           ///< 0x000000C8 .. 0x000000CB
    quint16 lbl14_rec_size = 0;         ///< 0x000000CC .. 0x000000CD
    quint8 byte0x000000CE_0x000000CF[2] = {0};
    quint32 lbl15_offset = 0;           ///< 0x000000D0 .. 0x000000D3
    quint32 lbl15_length = 0;           ///< 0x000000D4 .. 0x000000D7
    quint16 lbl15_rec_size = 0;         ///< 0x000000D8 .. 0x000000D9
    quint8 byte0x000000DA_0x000000DD[4] = {0};
    quint32 lbl16_offset = 0;           ///< 0x000000DE .. 0x000000E1
    quint32 lbl16_length = 0;           ///< 0x000000E2 .. 0x000000E5
    quint16 lbl16_rec_size = 0;         ///< 0x000000E6 .. 0x000000E7
    quint8 byte0x000000E8_0x000000EB[4] = {0};
    quint32 lbl17_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl17_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl17_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000000F6_0x000000F9[4] = {0};
    quint32 lbl18_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl18_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl18_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x00000104_0x00000107[4] = {0};
    quint32 lbl19_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl19_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl19_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x00000112_0x00000115[4] = {0};
    quint32 lbl20_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl20_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl20_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x00000120_0x00000123[4] = {0};
    quint32 lbl21_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl21_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl21_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x0000012E_0x00000131[4] = {0};
    quint32 lbl22_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl22_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl22_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x0000013C_0x0000013F[4] = {0};
    quint32 lbl23_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl23_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl23_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x0000014A_0x0000014D[4] = {0};
    quint32 lbl24_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl24_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl24_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x0000015A_0x0000015B[2] = {0};
    quint32 lbl25_offset;               ///< 0x000000EC .. 0x000000EF
    quint32 lbl25_length;               ///< 0x000000F0 .. 0x000000F3
    quint16 lbl25_rec_size;             ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x00000165_0x00000168[4] = {0};
    quint32 lbl26_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl26_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl26_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x00000172_0x00000175[4] = {0};
    quint32 lbl27_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl27_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl27_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x00000180_0x00000183[4] = {0};
    quint32 lbl28_offset = 0;           ///< 0x000000EC .. 0x000000EF   // list of JPG offsets into LBL29
    quint32 lbl28_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl28_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x0000018E_0x00000191[4] = {0};
    quint32 lbl29_offset = 0;           ///< 0x000000EC .. 0x000000EF   // all JPGs
    quint32 lbl29_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint32 lbl30_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl30_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl30_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001A4_0x000001A5[2] = {0};
    quint32 lbl31_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl31_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl31_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001B0_0x000001B1[2] = {0};
    quint32 lbl32_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl32_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl32_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001BC_0x000001BD[2] = {0};
    quint32 lbl33_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl33_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl33_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001C8_0x000001C9[2] = {0};
    quint32 lbl34_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl34_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl34_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001D4_0x000001D7[4] = {0};
    quint32 lbl35_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl35_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl35_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001E2_0x000001E5[4] = {0};
    quint32 lbl36_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl36_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl36_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001F0_0x000001F1[2] = {0};
    quint32 lbl37_offset = 0;           ///< 0x000000EC .. 0x000000EF
    quint32 lbl37_length = 0;           ///< 0x000000F0 .. 0x000000F3
    quint16 lbl37_rec_size = 0;         ///< 0x000000F4 .. 0x000000F5
    quint8 byte0x000001FC_0x000001FF[4] = {0};

    void print(quint32 offset);
};

struct hdr_net_t : public subfile_hdr_t
{
    hdr_net_t()
    {
        memset(this, 0, sizeof(hdr_net_t));
    }
};

struct hdr_nod_t : public subfile_hdr_t
{
    hdr_nod_t()
    {
        memset(this, 0, sizeof(hdr_nod_t));
    }
};

struct hdr_dem_t : public subfile_hdr_t
{
    hdr_dem_t()
    {
        memset(this, 0, sizeof(hdr_dem_t));
    }

    quint32 flags;
    quint16 zoomLevels;
    quint8 byte0x0000001B_0x0000001D[4];
    quint16 rec_size;
    quint32 offset_block3;
    quint8 byte0x00000025_0x00000029[4];

    void print(quint32 offset);
};

struct tre_1_t
{
    quint8 zoom;
    quint8 bits;
    quint16 subdiv;

    void print()
    {
        printf("zoom: %02X bits: %i subdiv: %i\n", zoom, bits, subdiv);
    }
};

// map subdivision definition, without pointer to the lower level subparts
struct tre_2_t
{
    quint24 rgn_offset;
    quint8 elements;
    quint24 center_lng;
    quint24 center_lat;
    quint16 width_trm;
#define TRE_SUBDIV_WIDTH(r)    (r->width_trm & 0x7FFF)
#define TRE_SUBDIV_TERM(r)     ((r->width_trm & 0x8000) != 0)
    quint16 height;
};

// pointer to the lower level subparts
struct tre_2_next_t : public tre_2_t
{
    quint16 next;
};


struct tre_7_t
{
    quint32 offsetPolygons;
    quint32 offsetPolyline;
    quint32 offsetPoints;
    quint8 btObjects;
};

struct maplevel_t
{
    maplevel_t()
    {
        memset(this, 0, sizeof(maplevel_t));
    }

    bool inherit;
    quint8 zoom;
    quint8 bits;
    quint16 subdiv;
};

struct subdiv_t
{
    subdiv_t()
    {
        memset(this, 0, sizeof(subdiv_t));
    }

    quint32 n;
    /// section of next level
    quint16 next;
    /// end of section group
    bool terminate;
    /// offset into the subfile's RGN part
    quint32 rgn_start;
    /// end of section in RGN part (last offset = rgn_end - 1)
    quint32 rgn_end;

    /// there are points stored in the RGN subsection
    bool hasPoints;
    /// there are indexd points stored in the RGN subsection
    bool hasIdxPoints;
    /// there are polylines stored in the RGN subsection
    bool hasPolylines;
    /// there are polygons stored in the RGN subsection
    bool hasPolygons;

    /// the center longitude of the area covered by this subdivision
    qint32 iCenterLng;
    /// the center latiude of the area covered by this subdivision
    qint32 iCenterLat;

    /// north boundary of area covered by this subsection []
    double north;
    /// east boundary of area covered by this subsection []
    double east;
    /// south boundary of area covered by this subsection []
    double south;
    /// west boundary of area covered by this subsection []
    double west;

    /// area in meter coordinates covered by this subdivision []
    QRectF area;

    /// number of left shifts for RGN data
    quint32 shift;
    /// map level this subdivision is shown
    quint32 level;

    quint32 offsetPolygons2;
    quint32 lengthPolygons2;
    quint32 offsetPoints2;
    quint32 lengthPoints2;
    quint32 offsetPolylines2;
    quint32 lengthPolylines2;

    maplevel_t * maplevel;

    quint32 nTiles;

    void print() const;
};

#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif


struct tile_t
{
    void print() const;

    quint32 offset = 0;
    double lon1 = 0;
    double lat1 = 0;
    double lon2 = 0;
    double lat2 = 0;

    quint32 w = 0;
    quint32 h = 0;

    double scaleX = 0.0;
    double scaleY = 0.0;

    QImage img;

    bool isSingleColor()
    {
        quint32 * p = (quint32*)img.bits();
        quint32 color = *p;
        const quint32 N = img.width() * img.height();
        for(quint32 n = 0; n < N; n++)
        {
            if(color != *p++)
            {
                return false;
            }
        }
        return true;
    }
};

struct subfile_part_t
{
    subfile_part_t() : offset(0), size(0){}
    quint32 offset;
    quint32 size;
};

struct subfile_t
{
    QString name;
    QMap<QString, subfile_part_t> parts;

    hdr_tre_t hdrTRE;
    hdr_rgn_t hdrRGN;
    hdr_lbl_t hdrLBL;
    hdr_nod_t hdrNOD;
    hdr_net_t hdrNET;
    hdr_dem_t hdrDEM;
    QVector<maplevel_t> mapLevels;
    QVector<subdiv_t>   subdivs;
    quint32 nSubdivsNext = 0;

    IGarminStrTbl * strtbl;
};

#define TRE_MAP_LEVEL(r) ((r)->zoom & 0x0f)
#define TRE_MAP_INHER(r) (((r)->zoom & 0x80) != 0)


