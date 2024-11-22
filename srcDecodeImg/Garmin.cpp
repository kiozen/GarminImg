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

#include "Garmin.h"

void printArrayUInt8(const char* label, quint8* array, int size) {
  printf("%30s ", label);
  for (int i = 0; i < size; i++) {
    printf("%02X ", array[i]);
  }
  printf("\n");
}

void printArrayInt8(const char* label, char* array, int size) {
  printf("%30s ", label);
  for (int i = 0; i < size; i++) {
    printf("%02X ", array[i]);
  }
  printf("| %s\n", QString(QByteArray(array, size)).toUtf8().data());
}

void printArrayInt8(const char* label, qint8* array, int size) {
  printf("%30s ", label);
  for (int i = 0; i < size; i++) {
    printf("%02X ", array[i]);
  }
  printf("\n");
}

void printInt8(const char* label, qint8 val) { printf("%30s %i\n", label, val); }
void printUInt8(const char* label, quint8 val) { printf("%30s %02X\n", label, val); }

void printInt16(const char* label, qint16 val) { printf("%30s %i\n", label, val); }

void printUInt16(const char* label, quint16 val) { printf("%30s %04X\n", label, val); }

void printUInt24(const char* label, quint24 val) {
  qint32 tmp = val[0] | val[1] << 8 | val[2] << 16;
  printf("%30s %f (0x%06X, %i)\n", label, GARMIN_DEG(tmp), tmp, tmp);
}

void printInt32(const char* label, qint32 val) { printf("%30s %i\n", label, val); }

void printUInt32(const char* label, quint32 val) { printf("%30s %08X\n", label, val); }

void gmapsupp_imghdr_t::print() {
  printUInt8("xorByte", xorByte);
  printArrayUInt8("byte0x00000001_0x00000007", byte0x00000001_0x00000007, sizeof(byte0x00000001_0x00000007));
  printUInt16("version", version);
  printInt8("upMonth", upMonth);
  printInt16("upYear", upYear + 1900);
  printArrayUInt8("byte0x0000000C_0x0000000D", byte0x0000000C_0x0000000D, sizeof(byte0x0000000C_0x0000000D));
  printUInt8("supp", supp);
  printUInt8("checksum", checksum);
  printArrayInt8("signature", signature, sizeof(signature));
  printUInt8("byte0x00000017", byte0x00000017);
  printUInt16("sectors1", sectors1);
  printUInt16("heads1", heads1);
  printUInt16("cylinders", cylinders);
  printArrayUInt8("byte0x0000001E_0x000000038", byte0x0000001E_0x000000038, sizeof(byte0x0000001E_0x000000038));
  printInt16("year", year);
  printInt8("month", month);
  printInt8("day", day);
  printInt8("hour", hour);
  printInt8("minute", minute);
  printInt8("second", second);
  printInt8("offsetFAT", offsetFAT);
  printArrayInt8("identifier", identifier, sizeof(identifier));
  printUInt8("byte0x00000048", byte0x00000048);
  printArrayInt8("desc1", desc1, sizeof(desc1));
  printUInt16("head2", head2);
  printUInt16("sectors2", sectors2);
  printUInt8("e1", e1);
  printUInt8("e2", e2);
  printUInt16("nBlocks1", nBlocks1);
  printArrayInt8("desc2", desc2, sizeof(desc2));
  printArrayUInt8("byte0x00000083_0x000001BE", byte0x00000083_0x000001BE, sizeof(byte0x00000083_0x000001BE));
  printUInt8("startHead", startHead);
  printUInt8("startSector", startSector);
  printUInt8("startCylinder", startCylinder);
  printUInt8("systemType", systemType);
  printUInt8("endHead", endHead);
  printUInt8("endSector", endSector);
  printUInt8("endCylinder", endCylinder);
  printUInt32("relSectors", relSectors);
  printUInt32("nSectors", nSectors);
  printArrayUInt8("byte0x0000001CE_0x000001FD", byte0x0000001CE_0x000001FD, sizeof(byte0x0000001CE_0x000001FD));
  printUInt16("terminator", terminator);
  // printArrayUInt8("byte0x00000200_0x00000FFF", byte0x00000200_0x00000FFF, sizeof(byte0x00000200_0x00000FFF));
}

void subfile_hdr_t::print() {
  printUInt16("size", size);
  printArrayInt8("type", type, sizeof(type));
  printUInt8("byte0x0000000C", byte0x0000000C);
  printUInt8("flag", flag);
  printInt16("year", year);
  printInt8("month", month);
  printInt8("day", day);
  printInt8("hour", hour);
  printInt8("minute", minute);
  printInt8("second", second);
}

void gmp_hdr_t::print() {
  subfile_hdr_t::print();
  printArrayUInt8("byte0x00000015_0x00000018", byte0x00000015_0x00000018, sizeof(byte0x00000015_0x00000018));
  printUInt32("offsetTRE", offsetTRE);
  printUInt32("offsetRGN", offsetRGN);
  printUInt32("offsetLBL", offsetLBL);
  printUInt32("offsetNET", offsetNET);
  printUInt32("offsetNOD", offsetNOD);
  printUInt32("offsetDEM", offsetDEM);
  printArrayUInt8("byte0x00000031_0x00000034", byte0x00000031_0x00000034, sizeof(byte0x00000031_0x00000034));
}

void hdr_tre_t::print(quint32 offset) {
  subfile_hdr_t::print();

  printUInt24("northbound", northbound);
  printUInt24("eastbound", eastbound);
  printUInt24("southbound", southbound);
  printUInt24("westbound", westbound);
  printUInt32("tre1_offset", tre1_offset + offset);
  printUInt32("tre1_size", tre1_size);
  printUInt32("tre2_offset", tre2_offset + offset);
  printUInt32("tre2_size", tre2_size);
  printUInt32("tre3_offset", tre3_offset + offset);
  printUInt32("tre3_size", tre3_size);
  printUInt16("tre3_rec_size", tre3_rec_size);
  printArrayUInt8("byte0x0000003B_0x0000003E", byte0x0000003B_0x0000003E, sizeof(byte0x0000003B_0x0000003E));
  printUInt8("POI_flags", POI_flags);
  printArrayUInt8("render_prio", render_prio, sizeof(render_prio));
  printArrayUInt8("byte0x00000043_0x00000049", byte0x00000043_0x00000049, sizeof(byte0x00000043_0x00000049));
  printUInt32("tre4_offset", tre4_offset + offset);
  printUInt32("tre4_size", tre4_size);
  printUInt16("tre4_rec_size", tre4_rec_size);
  printArrayUInt8("byte0x00000054_0x00000057", byte0x00000054_0x00000057, sizeof(byte0x00000054_0x00000057));
  printUInt32("tre5_offset", tre5_offset + offset);
  printUInt32("tre5_size", tre5_size);
  printUInt16("tre5_rec_size", tre5_rec_size);
  printArrayUInt8("byte0x00000062_0x00000065", byte0x00000062_0x00000065, sizeof(byte0x00000062_0x00000065));
  printUInt32("tre6_offset", tre6_offset + offset);
  printUInt32("tre6_size", tre6_size);
  printUInt16("tre6_rec_size", tre6_rec_size);
  printArrayUInt8("byte0x00000070_0x00000073", byte0x00000070_0x00000073, sizeof(byte0x00000070_0x00000073));
  printUInt32("map_id", map_id);
  printArrayUInt8("byte0x00000078_0x0000007B", byte0x00000078_0x0000007B, sizeof(byte0x00000078_0x0000007B));
  printUInt32("tre7_offset", tre7_offset + offset);
  printUInt32("tre7_size", tre7_size);
  printUInt16("tre7_rec_size", tre7_rec_size);
  printArrayUInt8("byte0x00000086_0x00000089", byte0x00000086_0x00000089, sizeof(byte0x00000086_0x00000089));
  printUInt32("tre8_offset", tre8_offset + offset);
  printUInt32("tre8_size", tre8_size);
  printUInt16("tre8_rec_size", tre8_rec_size);
  printUInt16("polyl2_types_num", polyl2_types_num);
  printUInt16("polyg2_types_num", polyg2_types_num);
  printUInt16("poi2_types_num", poi2_types_num);
  printArrayUInt8("key", key, sizeof(key));
  printArrayUInt8("byte0x000000AA_0x000000AD", byte0x000000AA_0x000000AD, sizeof(byte0x000000AA_0x000000AD));
  printUInt32("tre9_offset", tre9_offset + offset);
  printUInt32("tre9_size", tre9_size);
  printUInt16("tre9_rec_size", tre9_rec_size);
  printArrayUInt8("byte0x000000B8_0x000000BB", byte0x000000B8_0x000000BB, sizeof(byte0x000000B8_0x000000BB));
  printUInt32("tre10_offset", tre9_offset + offset);
  printUInt32("tre10_size", tre9_size);
  printUInt16("tre10_rec_size", tre9_rec_size);
  printArrayUInt8("byte0x000000C6_0x000000CE", byte0x000000C6_0x000000CE, sizeof(byte0x000000C6_0x000000CE));
  printUInt32("map_number", map_number);
}

void hdr_rgn_t::print(quint32 offset) {
  subfile_hdr_t::print();

  printUInt32("offset1", offset1 + offset);
  printUInt32("length1", length1);
  printUInt32("offset_polyg2", offset_polyg2 + offset);
  printUInt32("length_polyg2", length_polyg2);
  printArrayUInt8("byte0x00000025_0x00000038", byte0x00000025_0x00000038, sizeof(byte0x00000025_0x00000038));
  printUInt32("offset_polyl2", offset_polyl2 + offset);
  printUInt32("length_polyl2", length_polyl2);
  printArrayUInt8("byte0x00000041_0x00000054", byte0x00000041_0x00000054, sizeof(byte0x00000041_0x00000054));
  printUInt32("offset_point2", offset_point2 + offset);
  printUInt32("length_point2", length_point2);
  printArrayUInt8("byte0x0000005D_0x00000070", byte0x0000005D_0x00000070, sizeof(byte0x0000005D_0x00000070));
  printUInt32("offset2", offset2 + offset);
  printUInt32("length2", length2);
  printUInt32("unknown", unknown);
}

void hdr_lbl_t::print(quint32 offset) {
  subfile_hdr_t::print();

  printUInt32("lbl1_offset", lbl1_offset + offset);
  printUInt32("lbl1_length", lbl1_length);
  printUInt8("addr_shift", addr_shift);
  printUInt8("coding", coding);

  printUInt32("lbl2_offset", lbl2_offset + offset);
  printUInt32("lbl2_length", lbl2_length);
  printUInt16("lbl2_rec_size", lbl2_rec_size);
  printArrayUInt8("byte0x00000029_0x0000002C", byte0x00000029_0x0000002C, sizeof(byte0x00000029_0x0000002C));

  printUInt32("lbl3_offset", lbl3_offset + offset);
  printUInt32("lbl3_length", lbl3_length);
  printUInt16("lbl3_rec_size", lbl3_rec_size);
  printArrayUInt8("byte0x00000037_0x0000003A", byte0x00000037_0x0000003A, sizeof(byte0x00000037_0x0000003A));

  printUInt32("lbl4_offset", lbl4_offset + offset);
  printUInt32("lbl4_length", lbl4_length);
  printUInt16("lbl4_rec_size", lbl4_rec_size);
  printArrayUInt8("byte0x00000045_0x00000048", byte0x00000045_0x00000048, sizeof(byte0x00000045_0x00000048));

  printUInt32("lbl5_offset", lbl5_offset + offset);
  printUInt32("lbl5_length", lbl5_length);
  printUInt16("lbl5_rec_size", lbl5_rec_size);
  printArrayUInt8("byte0x00000053_0x00000056", byte0x00000053_0x00000056, sizeof(byte0x00000053_0x00000056));

  printUInt32("lbl6_offset", lbl6_offset + offset);
  printUInt32("lbl6_length", lbl6_length);
  printUInt8("lbl6_addr_shift", lbl6_addr_shift);
  printUInt8("lbl6_glob_mask", lbl6_glob_mask);
  printArrayUInt8("byte0x00000061_0x00000063", byte0x00000061_0x00000063, sizeof(byte0x00000061_0x00000063));

  printUInt32("lbl7_offset", lbl7_offset + offset);
  printUInt32("lbl7_length", lbl7_length);
  printUInt16("lbl7_rec_size", lbl7_rec_size);
  printArrayUInt8("byte0x0000006E_0x00000071", byte0x0000006E_0x00000071, sizeof(byte0x0000006E_0x00000071));

  printUInt32("lbl8_offset", lbl8_offset + offset);
  printUInt32("lbl8_length", lbl8_length);
  printUInt16("lbl8_rec_size", lbl8_rec_size);
  printArrayUInt8("byte0x0000007C_0x0000007F", byte0x0000007C_0x0000007F, sizeof(byte0x0000007C_0x0000007F));

  printUInt32("lbl9_offset", lbl9_offset + offset);
  printUInt32("lbl9_length", lbl9_length);
  printUInt16("lbl9_rec_size", lbl9_rec_size);
  printArrayUInt8("byte0x0000008A_0x0000008D", byte0x0000008A_0x0000008D, sizeof(byte0x0000008A_0x0000008D));

  printUInt32("lbl10_offset", lbl10_offset + offset);
  printUInt32("lbl10_length", lbl10_length);
  printUInt16("lbl10_rec_size", lbl10_rec_size);
  printArrayUInt8("byte0x00000098_0x0000009B", byte0x00000098_0x0000009B, sizeof(byte0x00000098_0x0000009B));

  printUInt32("lbl11_offset", lbl11_offset + offset);
  printUInt32("lbl11_length", lbl11_length);
  printUInt16("lbl11_rec_size", lbl11_rec_size);
  printArrayUInt8("byte0x000000A6_0x000000A9", byte0x000000A6_0x000000A9, sizeof(byte0x000000A6_0x000000A9));

  printUInt16("codepage", codepage);

  printArrayUInt8("byte0x000000AC_0x000000AF", byte0x000000AC_0x000000AF, sizeof(byte0x000000AC_0x000000AF));

  printUInt32("lbl12_offset", lbl12_offset + offset);
  printUInt32("lbl12_length", lbl12_length);

  printUInt32("lbl13_offset", lbl13_offset + offset);
  printUInt32("lbl13_length", lbl13_length);
  printUInt16("lbl13_rec_size", lbl13_rec_size);
  printArrayUInt8("byte0x000000C2_0x000000C3", byte0x000000C2_0x000000C3, sizeof(byte0x000000C2_0x000000C3));

  printUInt32("lbl14_offset", lbl14_offset + offset);
  printUInt32("lbl14_length", lbl14_length);
  printUInt16("lbl14_rec_size", lbl14_rec_size);
  printArrayUInt8("byte0x000000CE_0x000000CF", byte0x000000CE_0x000000CF, sizeof(byte0x000000CE_0x000000CF));

  printUInt32("lbl15_offset", lbl15_offset + offset);
  printUInt32("lbl15_length", lbl15_length);
  printUInt16("lbl15_rec_size", lbl15_rec_size);
  printArrayUInt8("byte0x000000DA_0x000000DD", byte0x000000DA_0x000000DD, sizeof(byte0x000000DA_0x000000DD));

  printUInt32("lbl16_offset", lbl16_offset + offset);
  printUInt32("lbl16_length", lbl16_length);
  printUInt16("lbl16_rec_size", lbl16_rec_size);
  printArrayUInt8("byte0x000000E8_0x000000EB", byte0x000000E8_0x000000EB, sizeof(byte0x000000E8_0x000000EB));

  printUInt32("lbl17_offset", lbl17_offset + offset);
  printUInt32("lbl17_length", lbl17_length);
  printUInt16("lbl17_rec_size", lbl17_rec_size);
  printArrayUInt8("byte0x000000F6_0x000000F9", byte0x000000F6_0x000000F9, sizeof(byte0x000000F6_0x000000F9));

  printUInt32("lbl18_offset", lbl18_offset + offset);
  printUInt32("lbl18_length", lbl18_length);
  printUInt16("lbl18_rec_size", lbl18_rec_size);
  printArrayUInt8("byte0x00000104_0x00000107", byte0x00000104_0x00000107, sizeof(byte0x00000104_0x00000107));

  printUInt32("lbl19_offset", lbl19_offset + offset);
  printUInt32("lbl19_length", lbl19_length);
  printUInt16("lbl19_rec_size", lbl19_rec_size);
  printArrayUInt8("byte0x00000112_0x00000115", byte0x00000112_0x00000115, sizeof(byte0x00000112_0x00000115));

  printUInt32("lbl20_offset", lbl20_offset + offset);
  printUInt32("lbl20_length", lbl20_length);
  printUInt16("lbl20_rec_size", lbl20_rec_size);
  printArrayUInt8("byte0x00000120_0x00000123", byte0x00000120_0x00000123, sizeof(byte0x00000120_0x00000123));

  printUInt32("lbl21_offset", lbl21_offset + offset);
  printUInt32("lbl21_length", lbl21_length);
  printUInt16("lbl21_rec_size", lbl21_rec_size);
  printArrayUInt8("byte0x0000012E_0x00000131", byte0x0000012E_0x00000131, sizeof(byte0x0000012E_0x00000131));

  printUInt32("lbl22_offset", lbl22_offset + offset);
  printUInt32("lbl22_length", lbl2_length);
  printUInt16("lbl22_rec_size", lbl22_rec_size);
  printArrayUInt8("byte0x0000013C_0x0000013F", byte0x0000013C_0x0000013F, sizeof(byte0x0000013C_0x0000013F));

  printUInt32("lbl23_offset", lbl23_offset + offset);
  printUInt32("lbl23_length", lbl23_length);
  printUInt16("lbl23_rec_size", lbl23_rec_size);
  printArrayUInt8("byte0x0000014A_0x0000014D", byte0x0000014A_0x0000014D, sizeof(byte0x0000014A_0x0000014D));

  printUInt32("lbl24_offset", lbl24_offset + offset);
  printUInt32("lbl24_length", lbl24_length);
  printUInt16("lbl24_rec_size", lbl24_rec_size);
  printArrayUInt8("byte0x0000015A_0x0000015B", byte0x0000015A_0x0000015B, sizeof(byte0x0000015A_0x0000015B));

  printUInt32("lbl25_offset", lbl25_offset + offset);
  printUInt32("lbl25_length", lbl25_length);
  printUInt16("lbl25_rec_size", lbl25_rec_size);
  printArrayUInt8("byte0x00000165_0x00000168", byte0x00000165_0x00000168, sizeof(byte0x00000165_0x00000168));

  printUInt32("lbl26_offset", lbl26_offset + offset);
  printUInt32("lbl26_length", lbl26_length);
  printUInt16("lbl26_rec_size", lbl26_rec_size);
  printArrayUInt8("byte0x00000172_0x00000175", byte0x00000172_0x00000175, sizeof(byte0x00000172_0x00000175));

  printUInt32("lbl27_offset", lbl27_offset + offset);
  printUInt32("lbl27_length", lbl27_length);
  printUInt16("lbl27_rec_size", lbl27_rec_size);
  printArrayUInt8("byte0x00000180_0x00000183", byte0x00000180_0x00000183, sizeof(byte0x00000180_0x00000183));

  printUInt32("lbl28_offset", lbl28_offset + offset);
  printUInt32("lbl28_length", lbl28_length);
  printUInt16("lbl28_rec_size", lbl28_rec_size);
  printArrayUInt8("byte0x0000018E_0x00000191", byte0x0000018E_0x00000191, sizeof(byte0x0000018E_0x00000191));

  printUInt32("lbl29_offset", lbl29_offset + offset);
  printUInt32("lbl29_length", lbl29_length);

  printUInt32("lbl30_offset", lbl30_offset + offset);
  printUInt32("lbl30_length", lbl30_length);
  printUInt16("lbl30_rec_size", lbl30_rec_size);
  printArrayUInt8("byte0x000001A4_0x000001A5", byte0x000001A4_0x000001A5, sizeof(byte0x000001A4_0x000001A5));

  printUInt32("lbl31_offset", lbl31_offset + offset);
  printUInt32("lbl31_length", lbl31_length);
  printUInt16("lbl31_rec_size", lbl31_rec_size);
  printArrayUInt8("byte0x000001B0_0x000001B1", byte0x000001B0_0x000001B1, sizeof(byte0x000001B0_0x000001B1));

  printUInt32("lbl32_offset", lbl32_offset + offset);
  printUInt32("lbl32_length", lbl32_length);
  printUInt16("lbl32_rec_size", lbl32_rec_size);
  printArrayUInt8("byte0x000001BC_0x000001BD", byte0x000001BC_0x000001BD, sizeof(byte0x000001BC_0x000001BD));

  printUInt32("lbl33_offset", lbl33_offset + offset);
  printUInt32("lbl33_length", lbl33_length);
  printUInt16("lbl33_rec_size", lbl33_rec_size);
  printArrayUInt8("byte0x000001C8_0x000001C9", byte0x000001C8_0x000001C9, sizeof(byte0x000001C8_0x000001C9));

  printUInt32("lbl34_offset", lbl34_offset + offset);
  printUInt32("lbl34_length", lbl34_length);
  printUInt16("lbl34_rec_size", lbl34_rec_size);
  printArrayUInt8("byte0x000001D4_0x000001D7", byte0x000001D4_0x000001D7, sizeof(byte0x000001D4_0x000001D7));

  printUInt32("lbl35_offset", lbl35_offset + offset);
  printUInt32("lbl35_length", lbl35_length);
  printUInt16("lbl35_rec_size", lbl35_rec_size);
  printArrayUInt8("byte0x000001E2_0x000001E5", byte0x000001E2_0x000001E5, sizeof(byte0x000001E2_0x000001E5));

  printUInt32("lbl36_offset", lbl36_offset + offset);
  printUInt32("lbl36_length", lbl36_length);
  printUInt16("lbl36_rec_size", lbl36_rec_size);
  printArrayUInt8("byte0x000001F0_0x000001F1", byte0x000001F0_0x000001F1, sizeof(byte0x000001F0_0x000001F1));

  printUInt32("lbl37_offset", lbl37_offset + offset);
  printUInt32("lbl37_length", lbl37_length);
  printUInt16("lbl37_rec_size", lbl37_rec_size);
  printArrayUInt8("byte0x000001FC_0x000001FF", byte0x000001FC_0x000001FF, sizeof(byte0x000001FC_0x000001FF));
}

void hdr_dem_t::print(quint32 offset) {
  subfile_hdr_t::print();

  printUInt32("flags", flags);
  printUInt16("zoomLevels", zoomLevels);
  printArrayUInt8("byte0x0000001B_0x0000001D", byte0x0000001B_0x0000001D, sizeof(byte0x0000001B_0x0000001D));
  printUInt16("rec_size", rec_size);
  printUInt32("offset_block3", offset_block3 + offset);
  printArrayUInt8("byte0x00000025_0x00000029", byte0x00000025_0x00000029, sizeof(byte0x00000025_0x00000029));
}

void subdiv_t::print() const {
  if (next) {
    printf("--- subdiv #%i next #%i---\n", n, next);
  } else {
    printf("--- subdiv #%i ---\n", n);
  }
  printf("north %f east  %f south %f west  %f\n", RAD_TO_DEG * north, RAD_TO_DEG * east, RAD_TO_DEG * south,
         RAD_TO_DEG * west);
  printf("shift %i level %i\n", shift, level);
  printf("rgn_start %08X rgn_end %08X\n", rgn_start, rgn_end);
  printf("Terminate %i hasPoints %i hasIdxPoints %i hasPolylines %i hasPolygons %i\n", terminate, hasPoints,
         hasIdxPoints, hasPolylines, hasPolygons);
  printf("offsetPolygons2:  %08X  lengthPolygons2:  %08X\n", offsetPolygons2, lengthPolygons2);
  printf("offsetPolylines2: %08X  lengthPolylines2: %08X\n", offsetPolylines2, lengthPolylines2);
  printf("offsetPoints2:    %08X  lengthPoints2:    %08X\n", offsetPoints2, lengthPoints2);
  printf("iCenterLng %f iCenterLat %f\n", GARMIN_DEG(iCenterLng), GARMIN_DEG(iCenterLat));
}

void tile_t::print() const {
  printf("%08X % 3i % 3i  %f %f %f %f\n", offset, img.width(), img.height(), lon1, lat1, lon2, lat2);
}
