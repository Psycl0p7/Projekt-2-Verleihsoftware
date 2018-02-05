#ifndef ENTRY_H
#define ENTRY_H

#include "datafield.h"
#include <QVector>

class Object
{
public:
    Object(QString category = NULL, QString barcode = NULL);
    QString getCategory();
    Datafield* getField(int index);
    QVector<Datafield*> getAllFields();
    QString getBarcode();
    void setBarcode(QString barcode);
    bool isRequired();
    bool removeField(int index);
    bool setFieldData(QString data);
    int countFields();
    int getFieldIndex(QString fieldname);
    void addField(Datafield *field);
    void setCategory(QString newName);
    void setFields(QVector<Datafield*> fields);
private:
    QString category;
    bool required;
    QVector<Datafield*> fields;
    QString barcode;
};

#endif // ENTRY_H
