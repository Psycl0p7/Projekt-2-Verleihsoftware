#include "category.h"

Entry::Entry(QString name)
{
    this->name = name;
}

int Entry::getAmountFields()
{
    return this->fields.count();
}

bool Entry::removeField(int index)
{
    bool ok = false;

    if(index >= this->fields.count() - 1) {
        ok = true;
        this->fields.removeAt(index);
    }

    return ok;
}

void Entry::addField(Datafield field)
{
    this->fields.append(field);
}

Datafield Entry::getField(int index)
{
    Datafield field = NULL;

    if(index >= this->fields.count() - 1) {
        field = this->fields.at(index);
    }

    return field;
}

int Entry::getFieldIndex(QString fieldname)
{
    int index = -1;

    for(int i = 0; i < this->fields.count();i++) {
        if(this->fields.at(i).getName() )
    }
}
