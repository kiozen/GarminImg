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

#include "CFileImg.h"

#include "CException.h"

CFileImg::CFileImg() {
  CFatBlock::setBlocksize(hdr.blocksize());
  blocks << CFatBlock();
}

CFileImg& CFileImg::operator<<(const QFileInfo& info) {
  blocks << CFatBlock(info);
  return *this;
}

void CFileImg::finalize() {
  CFatBlock& first = blocks.first();
  first.data_.size = blocks.size() * first.size() + hdr.offsetFAT();

  for (CFatBlock& block : blocks) {
    block.enumerate();
  }

  hdr.setBlockcount(CFatBlock::blockcount());
}

void CFileImg::write(QFile& file) {
  file.resize(CFatBlock::blockcount() * hdr.blocksize());
  file.seek(0);
  hdr.write(file);

  for (CFatBlock& block : blocks) {
    block.writeBlock(file);
  }

  for (CFatBlock& block : blocks) {
    block.writeData(file);
  }
}
