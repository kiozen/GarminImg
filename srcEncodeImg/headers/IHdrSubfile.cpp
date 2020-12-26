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

#include "CException.h"
#include "headers/IHdrSubfile.h"

IHdrSubfile::IHdrSubfile(quint16 size, const QString& typeSpecifier)
{
    common_.size = size;

    Q_ASSERT_X(typeSpecifier.size() == 3, "typeSpecifier", "Size of typeSpecifier must be 3");
    const QString& t = QString("GARMIN %1").arg(typeSpecifier);
    strncpy(common_.type, t.toLatin1().data(), sizeof(common_.type));

    const QDateTime& timestamp = QDateTime::currentDateTimeUtc();
    const QDate& date = timestamp.date();
    const QTime& time = timestamp.time();
    common_.year = date.year();
    common_.month = date.month();
    common_.day = date.day();
    common_.hour = time.hour();
    common_.minute = time.minute();
    common_.second = time.second();
}

void IHdrSubfile::write(QFile &file)
{
    file.write((const char*)&common_,  sizeof(common_));
}

quint32 IHdrSubfile::offset() const
{
    if(offset_ & 0xFFFFFFFF00000000LL)
    {
        throw CException("Offset of header exceeds 32 bit");
    }

    return offset_;
}

quint32 IHdrSubfile::size() const
{
    if(size_ & 0xFFFFFFFF00000000LL)
    {
        throw CException("Size of header exceeds 32 bit");
    }

    return size_;
}
