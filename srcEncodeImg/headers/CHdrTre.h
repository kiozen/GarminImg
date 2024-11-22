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

class CTre1;
class CTre2;
class CTre3;
class CTre7;
class CTre8;
class CSection;

class CHdrTre : public IHdrSubfile {
 public:
  CHdrTre();
  virtual ~CHdrTre() = default;

  void write(QFile& file) override;

  void setTr1(CTre1& section);
  void setTr2(CTre2& section);
  void setTr3(CTre3& section);
  void setTr4(CSection& section);
  void setTr5(CSection& section);
  void setTr6(CSection& section);
  void setTr7(CTre7& section);
  void setTr8(CTre8& section);
  void setTr9(CSection& section);
  void setTr10(CSection& section);

  quint8* key() { return data_.key; }

  void setBoundary(qreal northbound, qreal eastbound, qreal southbound, qreal westbound);
  void setMapId(const QString& str);
  quint32 mapId() const { return data_.map_id; }
  void setNumExtPolygons(quint16 n) { data_.polyg2_types_num = n; }

 protected:
#pragma pack(1)
  struct data_t {
    quint24 northbound = {0};  ///< 0x00000015 .. 0x00000017  // writer.put3s(area.getMaxLat());
    quint24 eastbound = {0};   ///< 0x00000018 .. 0x0000001A  // Utils.put3sLongitude(writer, area.getMaxLong());
    quint24 southbound = {0};  ///< 0x0000001B .. 0x0000001D  // writer.put3s(area.getMinLat());
    quint24 westbound = {
        0};  ///< 0x0000001E .. 0x00000020  // writer.put3s(area.getMinLong());  // minLong cant be +180
    quint32 tre1_offset = 0;                    ///< 0x00000021 .. 0x00000024  // writer.put4(getMapLevelsPos());
    quint32 tre1_size = 0;                      ///< 0x00000025 .. 0x00000028  // writer.put4(getMapLevelsSize());
    quint32 tre2_offset = 0;                    ///< 0x00000029 .. 0x0000002C  // writer.put4(getSubdivPos());
    quint32 tre2_size = 0;                      ///< 0x0000002D .. 0x00000030  // writer.put4(getSubdivSize());
    quint32 tre3_offset = 0;                    ///< 0x00000031 .. 0x00000034  // copyright.writeSectionInfo(writer);
    quint32 tre3_size = 0;                      ///< 0x00000035 .. 0x00000038  //
    quint16 tre3_rec_size = 0;                  ///< 0x00000039 .. 0x0000003A  //
    quint8 byte0x0000003B_0x0000003E[4] = {0};  // writer.put4(0);
    quint8 POI_flags = 0x01;                    ///< 0x0000003F                // writer.put1u(getPoiDisplayFlags());
    quint24 render_prio = {0x14, 0, 0x10};      ///< 0x00000040 .. 0x00000042  // 0x0014
    quint8 byte0x00000043_0x00000049[7] = {0x01, 0x08, 0x24, 0x00, 0x01, 0x00, 0x00};
    quint32 tre4_offset = 0;                    ///< 0x0000004A .. 0x0000004D  // polyline.writeSectionInfo(writer);
    quint32 tre4_size = 0;                      ///< 0x0000004E .. 0x00000051  //
    quint16 tre4_rec_size = 0;                  ///< 0x00000052 .. 0x00000053  //
    quint8 byte0x00000054_0x00000057[4] = {0};  // writer.put4(0);
    quint32 tre5_offset = 0;                    ///< 0x00000058 .. 0x0000005B  // polygon.writeSectionInfo(writer);
    quint32 tre5_size = 0;                      ///< 0x0000005C .. 0x0000005F  //
    quint16 tre5_rec_size = 0;                  ///< 0x00000060 .. 0x00000061  //
    quint8 byte0x00000062_0x00000065[4] = {0};  // writer.put4(0);
    quint32 tre6_offset = 0;                    ///< 0x00000066 .. 0x00000069  // points.writeSectionInfo(writer);
    quint32 tre6_size = 0;                      ///< 0x0000006A .. 0x0000006D  //
    quint16 tre6_rec_size = 0;                  ///< 0x0000006E .. 0x0000006F  //
    quint8 byte0x00000070_0x00000073[4] = {0};  // writer.put4(0);
    /*-----------------------------------------------------*/
    quint32 map_id = 0;                         ///< 0x00000074 .. 0x00000077  // writer.put4(getMapId());
    quint8 byte0x00000078_0x0000007B[4] = {0};  // writer.put4(0);
    // Object groups V2 (CTreGroup2).
    quint32 tre7_offset =
        0;  ///< 0x0000007C .. 0x0000007F  //Groups2Offset     // extTypeOffsets.writeSectionInfo(writer, true);
    quint32 tre7_size = 0;                                           ///< 0x00000080 .. 0x00000083  //dwGroups2Length
    quint16 tre7_rec_size = 0;                                       ///< 0x00000084 .. 0x00000085  //wGroup2RecSize
    quint8 byte0x00000086_0x00000089[4] = {0x01, 0x00, 0x00, 0x00};  // 01 00 00 00
    // Order: polyline, polygon, POI; each sorted by type (1 type 1 levels 1 subtype)
    quint32 tre8_offset = 0;       ///< 0x0000008A .. 0x0000008D  // extTypeOverviews.writeSectionInfo(writer);
    quint32 tre8_size = 0;         ///< 0x0000008E .. 0x00000091  //
    quint16 tre8_rec_size = 0;     ///< 0x00000092 .. 0x00000093  //
    quint16 polyl2_types_num = 0;  ///< 0x00000094 .. 0x00000095 // writer.put2u(numExtTypeLineTypes);
    quint16 polyg2_types_num = 0;  ///< 0x00000096 .. 0x00000097 // writer.put2u(numExtTypeAreaTypes);
    quint16 poi2_types_num = 0;    ///< 0x00000098 .. 0x00000099  // writer.put2u(numExtTypePointTypes);

    /*-----------------------------------------------------*/
    quint8 key[16] = {0};  ///< 0x0000009A .. 0x000000A5  // MapValues mv = new MapValues(mapId, getHeaderLength());
    quint8 byte0x000000AA_0x000000AD[4] = {0};  // writer.put4(0);
    quint32 tre9_offset;                        ///< 0x000000AE .. 0x000000B1  // writer.put4(0);
    quint32 tre9_size;                          ///< 0x000000B2 .. 0x000000B5  // writer.put4(0);
    quint16 tre9_rec_size;                      ///< 0x000000B6 .. 0x000000B7  // writer.put2u(0);
    quint8 byte0x000000B8_0x000000BB[4] = {0};  // writer.put4(0);
    quint32 tre10_offset;                       ///< 0x000000BC .. 0x000000BF //
    quint32 tre10_size;                         ///< 0x000000C0 .. 0x000000C3 //
    quint16 tre10_rec_size;                     ///< 0x000000C4 .. 0x000000C5 //
    quint8 byte0x000000C6_0x000000CE[9] = {0};  //
    quint32 map_number;                         ///< 0x00000CF .. 0x000000D2  // map number as hex
  };
#ifdef WIN32
#pragma pack()
#else
#pragma pack(0)
#endif
  data_t data_;
};
