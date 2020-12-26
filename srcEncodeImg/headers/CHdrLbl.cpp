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

#include "headers/CHdrLbl.h"
#include "sections/CLbl1.h"
#include "sections/CLbl12.h"
#include "sections/CLbl28.h"
#include "sections/CLbl29.h"
#include "sections/CSection.h"

constexpr auto HDR_LBL_SIZE = 0x0200;

CHdrLbl::CHdrLbl()
    : IHdrSubfile(HDR_LBL_SIZE, "LBL")
{
}

void CHdrLbl::write(QFile& file)
{
    if(offset_)
    {
        file.seek(offset_);
        IHdrSubfile::write(file);
        file.write((const char*)&data,  sizeof(data));
    }
    else
    {
        offset_ = file.pos();
        IHdrSubfile::write(file);
        file.write((const char*)&data,  sizeof(data));
        size_ = file.pos() - offset_;
    }
}

#define VAR(x, y) x ## y

#define SET_LBL_2(x, s) \
    data.VAR(x, _offset) = s.offset(); \
    data.VAR(x, _length) = s.size()

#define SET_LBL_3(x, s) \
    data.VAR(x, _offset) = s.offset(); \
    data.VAR(x, _length) = s.size(); \
    data.VAR(x, _rec_size) = s.sizeRecord()

void CHdrLbl::setLbl1(CLbl1& section)
{
    SET_LBL_2(lbl1, section);
}
void CHdrLbl::setLbl2(CSection& section)
{
    SET_LBL_3(lbl2, section);
}
void CHdrLbl::setLbl3(CSection& section)
{
    SET_LBL_3(lbl3, section);
}
void CHdrLbl::setLbl4(CSection& section)
{
    SET_LBL_3(lbl4, section);
}
void CHdrLbl::setLbl5(CSection& section)
{
    SET_LBL_3(lbl5, section);
}
void CHdrLbl::setLbl6(CSection& section)
{
    SET_LBL_2(lbl6, section);
}
void CHdrLbl::setLbl7(CSection& section)
{
    SET_LBL_3(lbl7, section);
}
void CHdrLbl::setLbl8(CSection& section)
{
    SET_LBL_3(lbl8, section);
}
void CHdrLbl::setLbl9(CSection& section)
{
    SET_LBL_3(lbl9, section);
}
void CHdrLbl::setLbl10(CSection& section)
{
    SET_LBL_3(lbl10, section);
}

void CHdrLbl::setLbl11(CSection& section)
{
    SET_LBL_3(lbl11, section);
}
void CHdrLbl::setLbl12(CLbl12& section)
{
    SET_LBL_2(lbl12, section);
}
void CHdrLbl::setLbl13(CSection& section)
{
    SET_LBL_3(lbl13, section);
}
void CHdrLbl::setLbl14(CSection& section)
{
    SET_LBL_3(lbl14, section);
}
void CHdrLbl::setLbl15(CSection& section)
{
    SET_LBL_3(lbl15, section);
}
void CHdrLbl::setLbl16(CSection& section)
{
    SET_LBL_3(lbl16, section);
}
void CHdrLbl::setLbl17(CSection& section)
{
    SET_LBL_3(lbl17, section);
}
void CHdrLbl::setLbl18(CSection& section)
{
    SET_LBL_3(lbl18, section);
}
void CHdrLbl::setLbl19(CSection& section)
{
    SET_LBL_3(lbl19, section);
}
void CHdrLbl::setLbl20(CSection& section)
{
    SET_LBL_3(lbl20, section);
}

void CHdrLbl::setLbl21(CSection& section)
{
    SET_LBL_3(lbl21, section);
}
void CHdrLbl::setLbl22(CSection& section)
{
    SET_LBL_3(lbl22, section);
}
void CHdrLbl::setLbl23(CSection& section)
{
    SET_LBL_3(lbl23, section);
}
void CHdrLbl::setLbl24(CSection& section)
{
    SET_LBL_3(lbl24, section);
}
void CHdrLbl::setLbl25(CSection& section)
{
    SET_LBL_3(lbl25, section);
}
void CHdrLbl::setLbl26(CSection& section)
{
    SET_LBL_3(lbl26, section);
}
void CHdrLbl::setLbl27(CSection& section)
{
    SET_LBL_3(lbl27, section);
}
void CHdrLbl::setLbl28(CLbl28& section)
{
    SET_LBL_3(lbl28, section);
}
void CHdrLbl::setLbl29(CLbl29 &section)
{
    SET_LBL_2(lbl29, section);
}
void CHdrLbl::setLbl30(CSection& section)
{
    SET_LBL_3(lbl30, section);
}

void CHdrLbl::setLbl31(CSection& section)
{
    SET_LBL_3(lbl31, section);
}
void CHdrLbl::setLbl32(CSection& section)
{
    SET_LBL_3(lbl32, section);
}
void CHdrLbl::setLbl33(CSection& section)
{
    SET_LBL_3(lbl33, section);
}
void CHdrLbl::setLbl34(CSection& section)
{
    SET_LBL_3(lbl34, section);
}
void CHdrLbl::setLbl35(CSection& section)
{
    SET_LBL_3(lbl35, section);
}
void CHdrLbl::setLbl36(CSection& section)
{
    SET_LBL_3(lbl36, section);
}
void CHdrLbl::setLbl37(CSection& section)
{
    SET_LBL_3(lbl37, section);
}
