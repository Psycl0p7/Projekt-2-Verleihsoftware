#include "object.h"

Object::Object(QString category, QString barcode)
{
    this->category = category;
    this->barcode = barcode;
}

QString Object::getCategory()
{
    return this->category;
}

bool Object::isRequired()
{
    return this->required;
}

int Object::countFields()
{
    return this->fields.count();
}

bool Object::removeField(int index)
{
    bool ok = false;

    if(index < this->fields.count()) {
        ok = true;
        this->fields.removeAt(index);
    }

    return ok;
}

void Object::addField(Datafield* field)
{
    this->fields.append(field);
}

Datafield* Object::getField(int index)
{
    Datafield* field = NULL;
    if(index < this->fields.count() && index > -1) {
        field = this->fields.at(index);
    }

    return field;
}

void Object::setCategory(QString newName)
{
    this->category = newName;
}

int Object::getFieldIndex(QString fieldname)
{
    int index = -1;

    for(int i = 0; i < this->fields.count();i++) {
        if(fieldname == this->fields.at(i)->getName()) {
            index = i;
            break;
        }
    }

    return index;
}

QVector<Datafield*> Object::getAllFields()
{
    return this->fields;
}

void Object::setFields(QVector<Datafield*> fields)
{
    this->fields = fields;
}


QString Object::getBarcode()
{
    return this->barcode;
}

void Object::setBarcode(QString barcode)
{
    this->barcode = barcode;
}

