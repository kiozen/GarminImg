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

#include "CFileLbl.h"
#include "CFileRgn.h"
#include "CFileTre.h"
#include "headers/CHdrGmp.h"
#include "headers/CHdrRgn.h"

class QImage;

class CFileGmp
{
public:
    CFileGmp(const QString& name);
    virtual ~CFileGmp() = default;

    void write(const QDir& path);

    void addCopyright(const QString& str);
    CSubdiv &addSubdiv(quint8 maplevel, qreal northbound, qreal eastbound, qreal southbound, qreal westbound);
    void addTile(const QString &path,
                 qreal northbound, qreal eastbound,
                 qreal southbound, qreal westbound,
                 QList<qint16> subdivNumbers);

    void remove(const QDir& path);

private:
    QString name;

    CHdrGmp hdrGmp;

    CFileTre tre;
    CFileRgn rgn;
    CFileLbl lbl;

    static QStringList copyright1;
    static QStringList copyright2;

    QByteArray copyrightData1;
    QByteArray copyrightData2;

    bool hasPolygon2Section = false;
    bool hasPolyline2Section = false;
    bool hasPoint2Section = false;
};
