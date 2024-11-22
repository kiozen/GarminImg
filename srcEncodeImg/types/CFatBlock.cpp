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

#include "types/CFatBlock.h"

#include "CException.h"

quint32 CFatBlock::blocksize_ = 0;
quint32 CFatBlock::blockcount_ = 0;

CFatBlock::CFatBlock() {
  memset(data_.name, 0x20, sizeof(data_.name));
  memset(data_.type, 0x20, sizeof(data_.type));
  memset(data_.blocks, 0xFF, sizeof(data_.blocks));
  data_.part = 3;
}

CFatBlock::CFatBlock(const QFileInfo& fileinfo) : fileinfo_(fileinfo) {
  memcpy(data_.name, fileinfo_.baseName().toUpper().toLatin1().data(), sizeof(data_.name));
  memcpy(data_.type, fileinfo_.suffix().toUpper().toLatin1().data(), sizeof(data_.type));
  memset(data_.blocks, 0xFF, sizeof(data_.blocks));
  data_.size = quint32(fileinfo_.size());
}

void CFatBlock::enumerate() {
  const quint16 N = qCeil(qreal(data_.size) / blocksize_);

  Q_ASSERT_X(N < 240, "enumerate", "Data does not fit into one FAT block.");

  for (quint16 n = 0; n < N; n++) {
    data_.blocks[n] = blockcount_++;
  }
}

void CFatBlock::writeBlock(QFile& file) { file.write((const char*)&data_, sizeof(data_)); }

void CFatBlock::writeData(QFile& file) {
  file.seek(data_.blocks[0] * blocksize_);

  if (!fileinfo_.isFile()) {
    return;
  }

  const QString& filename = fileinfo_.absoluteFilePath();
  if (filename.isEmpty()) {
    return;
  }

  QFile source(filename);
  if (!source.open(QIODevice::ReadOnly)) {
    throw CException("Failed to open file " + source.fileName());
  }

  file.write(source.readAll());
}
