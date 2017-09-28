#include "gereatetyp.h"

Gereatetyp::Gereatetyp(int id, QString name)
{
    this->name = name;
    this->id = id;
}

Gereatetyp::Gereatetyp()
{

}

QString Gereatetyp::getName()
{
    return this->name;
}

int Gereatetyp::getId()
{
    return this->id;
}
