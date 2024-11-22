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

#include "headers/CHdrRgn.h"

#include "sections/CRgn2.h"
#include "sections/CRgn3.h"
#include "sections/CRgn4.h"
#include "sections/CSection.h"

constexpr auto HDR_RGN_SIZE = 0x007D;

CHdrRgn::CHdrRgn() : IHdrSubfile(HDR_RGN_SIZE, "RGN") {}

void CHdrRgn::setRgn1(CSection& section) {
  data.offset1 = section.offset();
  data.length1 = section.size();
}

void CHdrRgn::setRgn2(CRgn2& section) {
  data.offset_polyg2 = section.offset();
  data.length_polyg2 = section.size();
}

void CHdrRgn::setRgn3(CRgn3& section) {
  data.offset_polyl2 = section.offset();
  data.length_polyl2 = section.size();
}

void CHdrRgn::setRgn4(CRgn4& section) {
  data.offset_point2 = section.offset();
  data.length_point2 = section.size();
}

void CHdrRgn::setRgn5(CSection& section) {
  data.offset2 = section.offset();
  data.length2 = section.size();
}

void CHdrRgn::write(QFile& file) {
  if (offset_) {
    file.seek(offset_);
    IHdrSubfile::write(file);
    file.write((const char*)&data, sizeof(data));
  } else {
    offset_ = file.pos();
    IHdrSubfile::write(file);
    file.write((const char*)&data, sizeof(data));
    size_ = file.pos() - offset_;
  }
}
