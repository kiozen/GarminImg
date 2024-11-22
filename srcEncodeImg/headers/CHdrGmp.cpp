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

#include "headers/CHdrGmp.h"

#include "CFileLbl.h"
#include "CFileRgn.h"
#include "CFileTre.h"
#include "headers/CHdrLbl.h"
#include "headers/CHdrRgn.h"

constexpr auto HDR_GMP_SIZE = 0x35;

CHdrGmp::CHdrGmp() : IHdrSubfile(HDR_GMP_SIZE, "GMP") {}

void CHdrGmp::write(QFile& file) {
  file.seek(0);
  IHdrSubfile::write(file);
  file.write((const char*)&data, sizeof(data));
  size_ = file.pos() - offset_;
}

void CHdrGmp::setTre(const CFileTre& tre) { data.offsetTRE = tre.header().offset(); }

void CHdrGmp::setRgn(const CFileRgn& rgn) { data.offsetRGN = rgn.header().offset(); }

void CHdrGmp::setLbl(const CFileLbl& lbl) { data.offsetLBL = lbl.header().offset(); }
