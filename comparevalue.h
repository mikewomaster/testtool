#ifndef COMPAREVALUE_H
#define COMPAREVALUE_H

#include <QString>

class compareValue
{
public:
    compareValue();
    QString modelName;
    QString SN;
    int powerFull;
    int powerHalf;
    int powerEmpty;
    short state;
};

#endif // COMPAREVALUE_H
