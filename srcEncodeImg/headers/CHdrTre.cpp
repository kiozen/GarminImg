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

#include "headers/CHdrTre.h"

#include "CException.h"
#include "sections/CSection.h"
#include "sections/CTre1.h"
#include "sections/CTre2.h"
#include "sections/CTre3.h"
#include "sections/CTre7.h"
#include "sections/CTre8.h"
#include "types/coords.h"

constexpr auto HDR_TRE_SIZE = 0x00D3;

CHdrTre::CHdrTre() : IHdrSubfile(HDR_TRE_SIZE, "TRE") {}

void CHdrTre::setBoundary(qreal northbound, qreal eastbound, qreal southbound, qreal westbound) {
  mapunit_t val = degToMapUnit(northbound);
  data_.northbound[0] = val & 0x0FF;
  data_.northbound[1] = (val >> 8) & 0x0FF;
  data_.northbound[2] = (val >> 16) & 0x0FF;

  val = degToMapUnit(eastbound);
  data_.eastbound[0] = val & 0x0FF;
  data_.eastbound[1] = (val >> 8) & 0x0FF;
  data_.eastbound[2] = (val >> 16) & 0x0FF;

  val = degToMapUnit(southbound);
  data_.southbound[0] = val & 0x0FF;
  data_.southbound[1] = (val >> 8) & 0x0FF;
  data_.southbound[2] = (val >> 16) & 0x0FF;

  val = degToMapUnit(westbound);
  data_.westbound[0] = val & 0x0FF;
  data_.westbound[1] = (val >> 8) & 0x0FF;
  data_.westbound[2] = (val >> 16) & 0x0FF;
}

void CHdrTre::setMapId(const QString& str) {
  bool ok = false;
  quint32 number = str.toUInt(&ok);
  if (!ok) {
    throw CException("GMP name must be an 8 digit decimal number.");
  }

  data_.map_number = number;
  data_.map_id = number;
}

void CHdrTre::write(QFile& file) {
  if (offset_) {
    file.seek(offset_);
    IHdrSubfile::write(file);
    file.write((const char*)&data_, sizeof(data_));
  } else {
    offset_ = file.pos();
    IHdrSubfile::write(file);
    file.write((const char*)&data_, sizeof(data_));
    size_ = file.pos() - offset_;
  }
}

void CHdrTre::setTr1(CTre1& section) {
  data_.tre1_offset = section.offset();
  data_.tre1_size = section.size();
}

void CHdrTre::setTr2(CTre2& section) {
  data_.tre2_offset = section.offset();
  data_.tre2_size = section.size();
}

void CHdrTre::setTr3(CTre3& section) {
  data_.tre3_offset = section.offset();
  data_.tre3_size = section.size();
  data_.tre3_rec_size = section.sizeRecord();
}

void CHdrTre::setTr4(CSection& section) {
  data_.tre4_offset = section.offset();
  data_.tre4_size = section.size();
  data_.tre4_rec_size = section.sizeRecord();
}

void CHdrTre::setTr5(CSection& section) {
  data_.tre5_offset = section.offset();
  data_.tre5_size = section.size();
  data_.tre5_rec_size = section.sizeRecord();
}

void CHdrTre::setTr6(CSection& section) {
  data_.tre6_offset = section.offset();
  data_.tre6_size = section.size();
  data_.tre6_rec_size = section.sizeRecord();
}

void CHdrTre::setTr7(CTre7& section) {
  data_.tre7_offset = section.offset();
  data_.tre7_size = section.size();
  data_.tre7_rec_size = section.sizeRecord();
}

void CHdrTre::setTr8(CTre8& section) {
  data_.tre8_offset = section.offset();
  data_.tre8_size = section.size();
  data_.tre8_rec_size = section.sizeRecord();
}

void CHdrTre::setTr9(CSection& section) {
  data_.tre9_offset = section.offset();
  data_.tre9_size = section.size();
  data_.tre9_rec_size = section.sizeRecord();
}

void CHdrTre::setTr10(CSection& section) {
  data_.tre10_offset = section.offset();
  data_.tre10_size = section.size();
  data_.tre10_rec_size = section.sizeRecord();
}
