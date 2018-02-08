#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QFile>
#include <QVariant>
#include <QDateTime>
#include <QCryptographicHash>
#include <QSqlError>
#include <QDebug>

#include "object.h"
#include "rental.h"

class DBHandler
{
public:
    DBHandler();

    bool readSupportedDatatypes(QSqlQuery* p_qry, QString *error);

    bool getCategories(QSqlQuery* p_qry, QString *error);
    bool createCategory(QString name, QString *error);
    bool updateCategory(QString name, QString newName, QString *error);
    bool checkCategoryExists(QString categoryName, bool* categoryExists, QString *error);
    bool deleteCategory(QString name, QString* error);

    bool getCustomfields(QSqlQuery* p_qry, QString *error, QString gereateTyp);
    bool createCustomField(QString *error, QString name, QString geraeteTyp, int datentyp, bool pflichtfeld);
    bool checkCustomfieldExists(QString fieldName, QString categoryName, bool* customfieldExists,QString *error);
    bool readCustomField(QString* error, QString category, QString fieldname, int* datatype, bool* required);
    bool updateCustomField(QString category, QString fieldname, QString newName, int newDatatype, bool newRequired, QString *error);
    bool deleteCustomField(QString category, QString fieldname, QString* error);

    bool getObjectByBarcode(QString barcode, Object* object, bool *found, QString* error);
    bool searchObjectsByCategory(QString category, QVector<Object *> *foundObjects, QString* error);

    bool checkObjectAvailability(QString barcode, bool *isAvailable, QString* error);
    bool createRental(Rental* rental, QString* error);
    bool checkBarcodeisAvailable(QString barcode, bool* isAvailable, QString* error);
    bool createObjects(QVector<Object*> objects, QString* error);
    bool insertObjectData(QVector<Object*> objects, QString* error);
    bool updateObjectData(QVector<Object*> objects, QString* error);
    bool deleteObjects(QVector<Object *> objects, QString* error);
    bool insertObjectDataForFieldCreation(QString category, QString fieldname, QString *error);

private:

    QSqlDatabase db;

    bool getFieldnamesByBarcode(QString barcode, QVector<QString>* fieldnames, QString *error);
    bool getObjectDataByBarcode (QString barcode, QString* category, QVector<QString>* data, QString *error);

    bool DBExists();
    bool createDB();
    bool openDB();
    bool closeDB();
    bool execute(QString statement, QSqlQuery* p_qry, QString *error);
};

#endif // DBHANDLER_H
