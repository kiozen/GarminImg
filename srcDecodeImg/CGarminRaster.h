#ifndef CGARMINRASTER_H
#define CGARMINRASTER_H

#include <QtCore>

class CGarminRaster
{
public:
    CGarminRaster();
    virtual ~CGarminRaster();

    quint32 decode(const quint8 * pData, const quint8 * pEnd);

    double lon1;
    double lat1;
    double lon2;
    double lat2;

    quint32 index;
    quint32 size;

private:
    quint32 getBlockLength(quint8 val);
};

#endif //CGARMINRASTER_H

