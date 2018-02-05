#include "entry.h"

Entry::Entry(QString category, QString barcode)
{
    this->category = category;
    this->barcode = barcode;
}

QString Entry::getCategory()
{
    return this->category;
}

bool Entry::isRequired()
{
    return this->required;
}

int Entry::countFields()
{
    return this->fields.count();
}

bool Entry::removeField(int index)
{
    bool ok = false;

    if(index < this->fields.count()) {
        ok = true;
        this->fields.removeAt(index);
    }

    return ok;
}

void Entry::addField(Datafield* field)
{
    this->fields.append(field);
}

Datafield* Entry::getField(int index)
{
    Datafield* field = NULL;
    if(index < this->fields.count() && index > -1) {
        field = this->fields.at(index);
    }

    return field;
}

void Entry::setCategory(QString newName)
{
    this->category = newName;
}

int Entry::getFieldIndex(QString fieldname)
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

QVector<Datafield*> Entry::getAllFields()
{
    return this->fields;
}

void Entry::setFields(QVector<Datafield*> fields)
{
    this->fields = fields;
}


QString Entry::getBarcode()
{
    return this->barcode;
}

void Entry::setBarcode(QString barcode)
{
    this->barcode = barcode;
}

