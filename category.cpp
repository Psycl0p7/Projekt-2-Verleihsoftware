#include "category.h"

Object::Object(QString name)
{
    this->name = name;
}

int Object::getAmountFields()
{
    return this->fields.count();
}

bool Object::removeField(int index)
{
    bool ok = false;

    if(index >= this->fields.count() - 1) {
        ok = true;
        this->fields.removeAt(index);
    }

    return ok;
}

void Object::addField(Datafield field)
{
    this->fields.append(field);
}

Datafield Object::getField(int index)
{
    Datafield field = NULL;

    if(index >= this->fields.count() - 1) {
        field = this->fields.at(index);
    }

    return field;
}

int Object::getFieldIndex(QString fieldname)
{
    int index = -1;

    for(int i = 0; i < this->fields.count();i++) {
        if(this->fields.at(i).getName() )
    }
}
