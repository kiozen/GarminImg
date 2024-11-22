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

#include <QtCore>

#include "CFuzzyMap.h"
#include "Garmin.h"

class GDALDriverManager;
class GDALDriver;

class CGarminDecoder : public QObject {
 public:
  CGarminDecoder(const QString &filename);
  virtual ~CGarminDecoder() = default;

  void loadBasics();
  void dumpSubfiles();
  void analyse();

 private:
  void print(const char *format, ...);
  void readFAT(QFile &file);
  void readSubfiles(QFile &file);
  void readGMP(QFile &file, subfile_t &subfile);
  void readMapLevels(QFile &file, subfile_t &subfile);
  void readSubdivInfo(QFile &file, subfile_t &subfile);
  void readSubdivInfoExt(QFile &file, subfile_t &subfile);
  void readStringTable(QFile &file, subfile_t &subfile);

  void readTre8(QFile &file, subfile_t &subfile);

  void readMPS(QFile &file, subfile_t &subfile);
  void readProductInfo(QDataStream &stream);
  void readMapInfo(QDataStream &stream);
  QString readRawString(QDataStream &stream);

  void readTRE(QFile &file, subfile_t &subfile);
  void readRGN(QFile &file, subfile_t &subfile);
  void readLBL(QFile &file, subfile_t &subfile);

  void processSubfile(const subfile_t &subfile);

  QString filename;
  QString basename;

  QMap<QString, subfile_t> subfiles;

  QMutex mutex;

  CFuzzyMap<QString> outputPaths;
  GDALDriverManager *drvman = nullptr;
  GDALDriver *driver = nullptr;
};
