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

#include "CEncoder.h"
#include "CMap.h"
#include "Garmin.h"
#include "Platform.h"

#include <QtCore>

#define MAX_NUMBER_FAT_ENTRIES 760

CEncoder::CEncoder(const QString &filename)
    : filename(filename)
{
}

CEncoder::~CEncoder()
{
}

void CEncoder::start(CMap& map)
{
    foreach(const CMap::subfile_t&  subfile, map.getSubfiles())
    {
        gmp_hdr_t hdrGmp;
        hdrGmp.setTimestamp();

        hdr_tre_t hdrTre;
        hdrTre.setTimestamp();

        hdr_rgn_t hdrRgn;
        hdrRgn.setTimestamp();

        hdr_lbl_t hdrLbl;
        hdrLbl.setTimestamp();
    }
}

void CEncoder::initImgHeader(CMap& map, gmapsupp_imghdr_t& hdr)
{
    int e1 = 9;
    int e2 = 7;
    int nBlocks = 0;
    int dataoffset = 0;

    // get the date
    QDateTime date = QDateTime::currentDateTime();

    // zero structure
    memset(&hdr, 0, sizeof(gmapsupp_imghdr_t));

    // space-fill the descriptor strings
    memset(&hdr.desc1, 0x20, sizeof(hdr.desc1));
    memset(&hdr.desc2, 0x20, sizeof(hdr.desc2) - 1);

    // put in current month and year
    hdr.upMonth = date.date().month();
    hdr.upYear  = date.date().year() - 1900;

    // copy the signature to the 7 byte string
    strncpy(hdr.signature, "DSKIMG", 7);

    // copy the identifier to the 7 byte string
    strncpy(hdr.identifier, "GARMIN", 7);

    // identify creator in the map description
    if(map.getName().size() <= sizeof(hdr.desc1))
    {
        memcpy(hdr.desc1, map.getName().toLatin1(), map.getName().size());
    }
    else
    {
        memcpy(hdr.desc1, map.getName().toLatin1(), sizeof(hdr.desc1));

        if(map.getName().size() <= (sizeof(hdr.desc1) + sizeof(hdr.desc2) - 1))
        {
            memcpy(hdr.desc2, map.getName().mid(sizeof(hdr.desc1)).toLatin1(), map.getName().size() - sizeof(hdr.desc1) - 1);
        }
        else
        {
            memcpy(hdr.desc2, map.getName().mid(sizeof(hdr.desc1)).toLatin1(), sizeof(hdr.desc2) - 1);
        }
    }



    // as far as is known, E1 is always 9, to force a minimum 512 block size
    hdr.e1 = e1;
    // the E2 corresponding to the optimum block size
    hdr.e2 = e2;

    // set the number of file blocks in the named field (note: unaligned!)
    gar_store(uint16_t, hdr.nBlocks1, (uint16_t) nBlocks);

    // add the "partition table" terminator
    hdr.terminator = gar_endian(uint16_t, 0xAA55);

    // add the data offset
    hdr.dataoffset = gar_endian(uint32_t, dataoffset);

    // create a pointer to set various unnamed fields
    quint8 * p = (quint8*)&hdr;

    // various - watch out for unaligned destinations
    // non-standard?
    *(p + 0x0E)               = 0x01;
    // standard value
    *(p + 0x17)               = 0x02;
    // standard is 0x0004
    *(uint16_t*)(p + 0x18)     = gar_endian(uint16_t, 0x0020);
    // standard is 0x0010
    *(uint16_t*)(p + 0x1A)     = gar_endian(uint16_t, 0x0020);
    // standard is 0x0020
    *(uint16_t*)(p + 0x1C)     = gar_endian(uint16_t, 0x03C7);
    // copies (0x1a)
    gar_ptr_store(uint16_t, p + 0x5D, 0x0020);
    // copies (0x18)
    gar_ptr_store(uint16_t, p + 0x5F, 0x0020);

    // date stuff
    gar_ptr_store(uint16_t, p + 0x39, date.date().year());
    *(p + 0x3B)               = date.date().month();
    *(p + 0x3C)               = date.date().day();
    *(p + 0x3D)               = date.time().hour();
    *(p + 0x3E)               = date.time().minute();
    *(p + 0x3F)               = date.time().second();
    // 0x02 standard. bit for copied map set?
    *(p + 0x40)               = 0x08;

    // more
    // copies (0x18) to a *2* byte field
    *(uint16_t*)(p + 0x1C4)    = gar_endian(uint16_t, 0x0020);
    /*  // not really needed ???
     *(p + 0x1C0)               = 0x01; // standard value
     *(p + 0x1C3)               = 0x15; // normal, but *not* (0x1A) - 1
     *(p + 0x1C4)               = 0x10; // value above
     *(p + 0x1C5)               = 0x00;
     */

    // set the number of file blocks at 0x1CA
    *(uint16_t*)(p + 0x1CA) = gar_endian(uint16_t, (uint16_t) nBlocks);
}

void CEncoder::startMPS(QDataStream& mps)
{
    QString name = "testmap";
    quint16 fid = 0xAF;
    quint16 pid = 0xFE;

    mps << quint8('F');
    mps << quint16(5 + name.toLatin1().size());
    // I suspect this should really be the basic file name of the .img set:
    mps << fid << pid;

    mps.writeRawData(name.toLatin1(), name.toLatin1().size());
    mps.writeRawData("\0", 1);
}

