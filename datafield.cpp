#include "datafield.h"

Datafield::Datafield(QString name, int type, bool required, QString data)
{
    this->name = name;
    this->type = type;
    this->required = required;
    this->data = data;
}

Datafield::Datafield(QString name, QString data)
{
    this->name = name;
    this->data = data;
    this->type = Datafield::TYPE_UNDEFINED;
    this->required = false;
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

bool Datafield::isRequired()
{
    return this->required;
}

// updates the meta data of datafield
void Datafield::updateMeta(QString name, int type, bool required)
{
    this->name = name;
    this->type = type;
    this->required = required;
}
