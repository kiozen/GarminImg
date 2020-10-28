
#include "CGarminRaster.h"
#include "Garmin.h"

CGarminRaster::CGarminRaster()
{
}

CGarminRaster::~CGarminRaster()
{
}

quint32 CGarminRaster::decode(const quint8 * pData, const quint8 * pEnd)
{
    quint8 type = *pData++;
    quint8 byte = *pData++;
    quint32 len = getBlockLength(byte);
    qint32 tmp32;


    index = *pData++;
    if(len == 22)
    {
        index |= (*pData++) << 8;
    }

    tmp32 = *(quint32*)pData;
    lat1 = GARMIN_DEG(tmp32 >> 8);
    pData += sizeof(tmp32);

    tmp32 = *(quint32*)pData;
    lon2 = GARMIN_DEG(tmp32 >> 8);
    pData += sizeof(tmp32);

    tmp32 = *(quint32*)pData;
    lat2 = GARMIN_DEG(tmp32 >> 8);
    pData += sizeof(tmp32);

    tmp32 = *(quint32*)pData;
    lon1 = GARMIN_DEG(tmp32 >> 8);
    pData += sizeof(tmp32);

    size = *(quint32*)pData;
    pData += sizeof(quint32);

//    printf("n %f e %f s %f w %f index: %i size: %i\n", lat1, lon2, lat2, lon1, index, size);

    return len + 2;
}


static const quint32 var_fld_extract_mask[8]  = { 0x1FFFFFFF, 0x7F, 0x3FFF, 0x7F, 0x1FFFFF, 0x7F, 0x3FFF, 0x7F };
static const quint8 var_fld_extract_bytes[8] = {          4,    1,      2,    1,        3,    1,      2,    1 };
static const quint8 var_fld_extract_rshft[8] = {          3,    1,      2,    1,        3,    1,      2,    1 };

quint32 CGarminRaster::getBlockLength(quint8 val)
{
    quint32 CurValue   = val;
    quint8 Lower3Bits = CurValue & 7;

    quint32 BlockLength = CurValue >> var_fld_extract_rshft[Lower3Bits];
    BlockLength &= var_fld_extract_mask[Lower3Bits];

    return BlockLength;
}
