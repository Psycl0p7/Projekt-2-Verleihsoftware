#ifndef ENTRY_H
#define ENTRY_H

#include "datafield.h"
#include <QVector>

class Entry
{
public:
    Entry(QString name);
    QString getName();
    bool isRequired();
    int getAmountFields();
    bool removeField(int index);
    void addField(Datafield *field);
    Datafield getField(int index);
    bool setFieldData(QString data);
    int getFieldIndex(QString fieldname);
private:
    QString name;
    bool required;
    QVector<Datafield*> fields;
};

#endif // ENTRY_H
