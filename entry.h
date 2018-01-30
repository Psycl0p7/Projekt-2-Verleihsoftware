#ifndef ENTRY_H
#define ENTRY_H

#include "datafield.h"
#include <QVector>

class Entry
{
public:
    Entry(QString name);
    QString getName();
    Datafield* getField(int index);
    QVector<Datafield*> getAllFields();
    bool isRequired();
    bool removeField(int index);
    bool setFieldData(QString data);
    int countFields();
    int getFieldIndex(QString fieldname);
    void addField(Datafield *field);
    void setName(QString newName);
    void setFields(QVector<Datafield*> fields);
private:
    QString name;
    bool required;
    QVector<Datafield*> fields;
};

#endif // ENTRY_H
