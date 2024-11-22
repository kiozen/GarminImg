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

#include "sections/CLbl29.h"

#include "CException.h"

CLbl29::CLbl29() {}

std::tuple<quint32, quint32> CLbl29::add(const QString& path) {
  quint32 offset = data_.size();

  QFile tile(path);
  if (!tile.open(QIODevice::ReadOnly)) {
    throw CException("Failed to create: " + tile.fileName());
  }

  const QByteArray& jpeg = tile.readAll();

  data_.append(jpeg);
  tile.close();

  return {offset, tile.size()};
}
