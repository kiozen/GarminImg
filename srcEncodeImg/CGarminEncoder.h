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

#include <proj_x.h>

#include <QMutex>
#include <QPointF>
#include <QString>
#include <QTemporaryDir>
#include <QTransform>

#include "types/CSubdiv.h"

class CFileGmp;

class GDALDriverManager;
class GDALDriver;

class CGarminEncoder {
 public:
  CGarminEncoder(const QString& filename);
  virtual ~CGarminEncoder() = default;

  void splitIntoTiles();
  void createImage(const QString& filename, const QString& description);

 private:
  void print(const char* format, ...);
  void exportSubfileTile(quint32 xoff, quint32 yoff, quint32 xsize, quint32 ysize, const QString& filename,
                         const QString& subfileName);
  bool isSingleColor(const QImage& img);
  void saveTile(const QImage& img, const QTransform& trFwd, quint32 x, quint32 y, quint32 xsizeTile, quint32 ysizeTile,
                const QDir& path, const CProj& proj, quint32 tileCount, CFileGmp& gmpFile,
                const QList<CSubdiv::number_t>& subdivNumbers);
  QMutex mutex;

  QString filename;
  QString basename;
  QTemporaryDir tmpdir;

  QPointF ref1;
  QPointF ref2;
  QPointF ref3;
  QPointF ref4;
};
