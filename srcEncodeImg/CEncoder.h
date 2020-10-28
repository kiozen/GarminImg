/**********************************************************************************************
    Copyright (C) 2013 Oliver Eichler oliver.eichler@gmx.de

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
#ifndef CENCODER_H
#define CENCODER_H

#include <QString>

struct gmapsupp_imghdr_t;
class QDataStream;
class CMap;

class CEncoder
{
public:
    CEncoder(const QString& filename);
    virtual ~CEncoder();

    void start(CMap &map);

private:
    void initImgHeader(CMap &map, gmapsupp_imghdr_t &hdr);

    void startMPS(QDataStream& mps);

    QString filename;
};

#endif //CENCODER_H

