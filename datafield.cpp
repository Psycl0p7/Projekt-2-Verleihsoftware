#include "datafield.h"

Datafield::Datafield(QString name, int type, QString data)
{
    this->name = name;
    this->type = type;
    this->data = data;
}

QString Datafield::getName()
{
    return this->name;
}

int Datafield::getType()
{
    return this->type;
}

QString Datafield::getData()
{
    return this->data;
}

void Datafield::setData(QString data)
{
    this->data = data;
}
