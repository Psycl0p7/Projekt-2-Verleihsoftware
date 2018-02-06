#ifndef ENTRY_H
#define ENTRY_H

#include "datafield.h"
#include <QString>
#include <QVector>

class Object
{
public:
    Object(QString name);
    int getAmountFields();
    bool removeField(int index);
    void addField(Datafield field);
    Datafield getField(int index);
    bool setFieldData(QString data);
    int getFieldIndex(QString fieldname);
private:
    QString name;
    QVector<Datafield> fields;

};

#endif // ENTRY_H
