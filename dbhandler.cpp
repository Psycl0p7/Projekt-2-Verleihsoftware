#include "dbhandler.h"
#include <qdebug.h>

/********************************************************************************
 *                              INIT-METHODEN                                   *
 ********************************************************************************/

DBHandler::DBHandler()
{
    createDB();
}

bool DBHandler::DBExists()
{
    bool dbExists = false;

    QFile db;
    db.setFileName("db.sqlite");
    if(db.exists()) {
        dbExists = true;
    }

    return dbExists;
}

/* was previouslyyeaah
 *
 * tbl_geraetetypen   -> tbl_categories
 * tbl_customfelder   -> tbl_datafields
 * tbl_geraeteData    -> tbl_objectdata
 * tbl_feldDatentypen -> tbl_datatypes
 * tbl_geraete        -> tbl_objects
 */

bool DBHandler::createDB()
{
    bool dbCreated = false;
    if(!DBExists()) {
        QString path = "db.sqlite";
        QString tblCategories   = "CREATE TABLE 'tbl_categories'    ( 'id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL UNIQUE )";
        QString tblDatafields   = "CREATE TABLE 'tbl_datafields'    ( 'id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL, 'fk_category' INTEGER NOT NULL, 'fk_datatype' INTEGER NOT NULL, 'required' BOOLEAN NOT NULL)";
        QString tblObjectdata   = "CREATE TABLE 'tbl_objectdata'    ( 'id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'fk_datafield' INTEGER, 'fk_object' INTEGER NOT NULL, 'data' TEXT DEFAULT '')";
        QString tblDatatypes    = "CREATE TABLE 'tbl_datatypes'     ( 'id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'name' TEXT);";
        QString tblEntries      = "CREATE TABLE 'tbl_objects'       ( 'id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'fk_category' INTEGER, 'barcode' TEXT NOT NULL UNIQUE);";
        QString tblRentals      = "CREATE TABLE 'tbl_rentals'       ( 'id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'firstname' TEXT NOT NULL, 'lastname' TEXT NOT NULL, 'extra' TEXT, 'start' TEXT, end TEXT);";
        QString tblRentalObject = "CREATE TABLE 'tbl_rental_object' ( 'fk_rental' INTEGER NOT NULL, 'fk_object' INTEGER NOT NULL UNIQUE);";


        this->db = QSqlDatabase::addDatabase("QSQLITE");
        this->db.setDatabaseName(path);
        this->db.open();

        QSqlQuery query;

        query.exec(tblCategories);
        query.exec(tblDatafields);
        query.exec(tblObjectdata);
        query.exec(tblDatatypes);
        query.exec(tblEntries);
        query.exec(tblRentals);
        query.exec(tblRentalObject);

        QString addDatentypText = "INSERT INTO tbl_datatypes (name) VALUES('Text')";

        query.exec(addDatentypText);
        db.close();

        dbCreated = true;
    }
    else if(DBExists()) {
        this->db = QSqlDatabase::addDatabase("QSQLITE");
        this->db.setDatabaseName("db.sqlite");
    }

    return dbCreated;
}

bool DBHandler::openDB()
{
    bool isOpen = false;
    db.open();

    if(this->db.isOpen()) {
        isOpen = true;
    }

    return isOpen;
}

bool DBHandler::closeDB()
{
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(QSqlDatabase::defaultConnection);

    return true;
}

bool DBHandler::execute(QString statement, QSqlQuery *p_qry, QString *error)
{
    bool ok = false;

    if(!this->openDB()) {
        *error = "Datenbank konnte nicht geöffnet werden.";
    }
    else {
        p_qry->prepare(statement);

        if(!p_qry->exec()) {
            *error = "Datenbankabfrage konnte nicht ausgeführt werden: " + p_qry->lastError().text();
        }
        else {
            ok = true;
        }
    }

    return ok;
}

/********************************************************************************
 *                              DB-Anfragen                                     *
 ********************************************************************************/


bool DBHandler::readSupportedDatatypes(QSqlQuery* p_qry, QString *error)
{
    QString statement = "SELECT name FROM tbl_datatypes ORDER BY name ASC";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::getCategories(QSqlQuery* p_qry, QString* error)
{
    QString statement = "SELECT name FROM tbl_categories ORDER BY name ASC";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::createCategory(QString name,QString* error)
{
    QString statement = "INSERT INTO tbl_categories (name) VALUES ('" + name + "')";
    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::updateCategory(QString name, QString newName, QString* error)
{
    QString statement = "UPDATE tbl_categories SET name='"
            + newName
            + "' WHERE name='"
            + name
            + "';";
    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::deleteCategory(QString name, QString *error)
{
    bool ok = true;
    QSqlQuery qry;
    QVector<QString> barcodes;
    QString getObjectBarcodes = "SELECT barcode FROM tbl_objects WHERE fk_category=(SELECT id FROM tbl_categories WHERE name='" + name + "');";
    QString deleteCategory = "DELETE FROM tbl_categories WHERE name='"
            + name
            + "';";

    if(!this->execute(getObjectBarcodes, &qry, error)) {
        ok = false;
    }
    else {
        while(qry.next()) {
            barcodes.append(qry.value(0).toString());
        }

        if(!this->deleteObjects(barcodes, error)) {
            ok = false;
        }
        else if(!this->execute(deleteCategory, &qry, error)) {
            ok = false;
        }
    }

    return ok;

}

bool DBHandler::checkCategoryExists(QString categoryName, bool *categoryExists, QString *error)
{
    bool ok = false;
    QSqlQuery qry;
    QString statement = "SELECT name FROM tbl_categories WHERE name='"
            + categoryName
            + "';";

    if(this->execute(statement, &qry, error)) {
        ok = true;
        if(!qry.first())
            *categoryExists = false;
    }
    return ok;
}


bool DBHandler::checkCustomfieldExists(QString fieldName, QString categoryName, bool* customfieldExists,QString *error)
{
    bool ok = false;
    QSqlQuery qry;
    QString statement = "SELECT name FROM tbl_datafields WHERE name='"
            + fieldName
            + "' AND fk_category=(SELECT id FROM tbl_categories WHERE name='"
            + categoryName
            + "');";

    if(this->execute(statement, &qry, error)) {
        ok = true;
        if(qry.first())
            *customfieldExists = true;
    }
    return ok;
}

bool DBHandler::getCustomfields(QSqlQuery* p_qry, QString *error, QString gereateTyp)
{
    QString statement = QString("SELECT name, fk_datatype, required FROM tbl_datafields WHERE fk_category="
                                "(SELECT id FROM tbl_categories WHERE name='") + gereateTyp + "')";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::createCustomField(QString *error, QString name, QString geraeteTyp, int datentyp, bool required)
{
    QString statement = "INSERT INTO tbl_datafields (name, fk_category,fk_datatype, required) VALUES('"
            + name + "',"
            + "(SELECT id FROM tbl_categories WHERE name='" + geraeteTyp + "'),"
            + QString::number(datentyp) + ","
            + QString::number(required)
            + ");";

    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::readCustomField(QString* error, QString category, QString fieldname, int *datatype, bool* required)
{
    bool ok = false;
    QSqlQuery qry;
    QString statement = "SELECT fk_datatype, required FROM tbl_datafields WHERE name='"
            + fieldname + "' AND fk_category=(SELECT id FROM tbl_categories WHERE name='" + category + "');";

    if(this->execute(statement, &qry, error)) {
        if(!qry.first()) {
            *error = "Datenfeld [" + category + " - " + fieldname +"] konnte nicht gefunden werden.";
        }
        else {
            *datatype = qry.value(0).toInt();
            *required = qry.value(1).toBool();
            ok = true;
        }
    }
    return ok;
}

bool DBHandler::updateCustomField(QString category, QString fieldname, QString newName, int newDatatype, bool newRequired, QString *error)
{
    QString statement = "UPDATE tbl_datafields SET name='"
            + newName
            + "',fk_datatype=" + QString::number(newDatatype)
            + ",required=" + QString::number(newRequired)
            + " WHERE fk_category=(SELECT id FROM tbl_categories WHERE name='" + category
            + "') AND name='" + fieldname + "';";
    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::deleteCustomField(QString category, QString fieldname, QString* error)
{
    QSqlQuery qry;
    bool ok = true;
    QString deleteData = "DELETE FROM tbl_objectdata WHERE fk_datafield=(SELECT id FROM tbl_datafields WHERE name='" + fieldname + "');";
    QString deleteField = "DELETE FROM tbl_datafields WHERE name='"
            + fieldname
            + "' AND fk_category=(SELECT id FROM tbl_categories WHERE name='"
            + category
            + "');";

    if(!this->execute(deleteData, &qry, error)) {
        ok = false;
    }
    else if(!this->execute(deleteField, &qry, error)) {
        ok = false;
    }
    return ok;
}

bool DBHandler::getObjectByBarcode(QString barcode, Object *object, bool *found, QString* error)
{
    bool ok = true;
    QSqlQuery qry;

    // object construction data
    QString category;

    QString statement = "SELECT DISTINCT"
            " tbl_categories.name AS category,"
            " (SELECT name FROM tbl_datafields WHERE id = tbl_objectdata.fk_datafield) AS fieldname,"
            " (SELECT required FROM tbl_datafields WHERE id = tbl_objectdata.fk_datafield) AS required,"
            " tbl_objectdata.data AS data"
            " FROM tbl_objects"
            " INNER JOIN tbl_categories"
            " ON tbl_categories.id = tbl_objects.fk_category"
            " INNER JOIN tbl_objectdata"
            " ON tbl_objectdata.fk_object = tbl_objects.id"
            " WHERE barcode = '" + barcode + "'"
            " ORDER BY fieldname ASC;";
    *found = false;

    if(!this->execute(statement, &qry, error)) {
        ok = false;
    }
    else {
        while(qry.next()) {
            *found = true;
            category = qry.value(0).toString();
            object->addField(new Datafield(qry.value(1).toString(), Datafield::TYPE_UNDEFINED, qry.value(2).toBool(), qry.value(3).toString()));
        }

        // create Object object
        object->setBarcode(barcode);
        object->setCategory(category);
    }

    return ok;
}

bool DBHandler::createRental(Rental* rental, QString* error)
{
    // human readable date time format
    bool ok = true;
    QSqlQuery qry;

    QString dateTimeFormat = "dd.MM.yyyy hh:mm";
    QString rentalId = "";

    // create rental entry
    QString statement = "INSERT INTO tbl_rentals(firstname, lastname, extra, start, end) VALUES('"
            + rental->getFirstname() + "', '"
            + rental->getLastname()  + "', '"
            + rental->getExtra()     + "', '"
            + rental->getStart().toString(dateTimeFormat) + "', '"
            + rental->getEnd().toString(dateTimeFormat) + "');";

    if(!this->execute(statement, new QSqlQuery(), error)) {
        ok = false;
    }
    else if(rental->countObjects() > 0) {
        if(!this->execute("SELECT MAX(id) FROM tbl_rentals;", &qry, error)){
            ok = false;
        }
        else if(!qry.first()) {
            ok = false;
        }
        else {
            // fetch id of inserted rental entry
            rentalId = QString::number(qry.value(0).toInt());
            // link objects
            statement = "INSERT INTO tbl_rental_object(fk_rental, fk_object) VALUES";
            for(int i = 0; i < rental->countObjects(); i++) {
                statement += "(" + rentalId + ", (SELECT id FROM tbl_objects WHERE barcode='" + rental->getObject(i)->getBarcode() + "'))";

                if(i < rental->countObjects() -1) {
                    statement += ", ";
                } else {
                    statement += ";";
                }
            }

            if(!this->execute(statement, new QSqlQuery(), error)) {
                ok = false;
            }
        }
    }

    return ok;
}

bool DBHandler::checkObjectAvailability(QString barcode, bool* isAvailable, QString *error)
{
    QSqlQuery* qry= new QSqlQuery();
    bool ok = false;
    QString statement = "SELECT COUNT(fk_object) FROM tbl_rental_object WHERE fk_object="
            "(SELECT id FROM tbl_objects WHERE barcode='" + barcode + "');";

    *isAvailable = false;
    if(this->execute(statement, qry, error)) {
        if(qry->first()) {
            ok = true;
            if(qry->value(0).toInt() < 1) {
                *isAvailable = true;
            }
        }
    }
    return ok;
}

bool DBHandler::searchObjectsByCategory(QString category, QVector<Object*>* foundObjects, QString* error)
{
    QSqlQuery* qry = new QSqlQuery();
    bool ok = false;
    QVector<QString> barcodes;
    QString statement = "SELECT barcode FROM tbl_objects WHERE fk_category=(SELECT id FROM tbl_categories WHERE name='" + category + "');";
    bool found = true;
    Object* foundObject = NULL;

    if(this->execute(statement, qry, error)) {
        ok = true;
        // prepare barcodes
        while(qry->next()) {
            barcodes.append(qry->value(0).toString());
        }

        for(int i = 0; i < barcodes.count(); i++) {
            foundObject = new Object();
            if(!this->getObjectByBarcode(barcodes.at(i), foundObject, &found, error)) {
                ok = false;
            }
            else if (!found) {
                found = false;
            }
            else {
                foundObjects->append(foundObject);
            }
        }
    }

    return ok;
}

bool DBHandler::checkBarcodeisAvailable(QString barcode, bool* isAvailable, QString* error)
{
    QSqlQuery* qry= new QSqlQuery();
    bool ok = false;
    QString statement = "SELECT COUNT(barcode) FROM tbl_objects WHERE barcode='"
            + barcode + "';";

    *isAvailable = false;
    if(this->execute(statement, qry, error)) {
        if(qry->first()) {
            ok = true;
            if(qry->value(0).toInt() == 0) {
                *isAvailable = true;
            }
        }
    }
    return ok;
}

bool DBHandler::createObjects(QVector<Object*> objects, QString* error)
{
    bool ok = true;;
    // create objects entries;
    QString statementCreateObjects = "INSERT INTO tbl_objects(barcode, fk_category) VALUES";
    QSqlQuery qry;

    // compile sql statements
    for(int objectIndex = 0; objectIndex < objects.count(); objectIndex++) {
        statementCreateObjects += "('" + objects.at(objectIndex)->getBarcode() + "', "
                + "(SELECT id FROM tbl_categories WHERE name='" + objects.at(objectIndex)->getCategory() + "'))";

        if(objectIndex < objects.count() - 1) {
            statementCreateObjects += ',';
        }
        else {
            statementCreateObjects += ";";
        }
    }
    // execute
    if(!this->execute(statementCreateObjects, &qry, error)) {
        ok = false;
    }

    return ok;
}

bool DBHandler::insertObjectData(QVector<Object*> objects, QString *error)
{
    bool ok = true;
    QVector<QString> statementsLinkData;
    QString statementLinkData;
    QSqlQuery qry;


    for(int objectIndex = 0; objectIndex < objects.count(); objectIndex++) {
        for(int fieldIndex = 0; fieldIndex < objects.at(objectIndex)->countFields(); fieldIndex++) {
            statementLinkData = "INSERT INTO tbl_objectdata(fk_object, fk_datafield, data) VALUES"
                "((SELECT id FROM tbl_objects WHERE barcode='" + objects.at(objectIndex)->getBarcode() + "')"
                ",(SELECT id FROM tbl_datafields WHERE fk_category=(SELECT id FROM tbl_categories WHERE name='"
                    + objects.at(objectIndex)->getCategory() + "') AND name='" + objects.at(objectIndex)->getField(fieldIndex)->getName() + "')"
                ",'" + objects.at(objectIndex)->getField(fieldIndex)->getData() + "');";

            statementsLinkData.append(statementLinkData);
        }
    }

    for(int i = 0; i < statementsLinkData.count(); i++) {
        if(!this->execute(statementsLinkData.at(i), &qry, error)) {
            ok = false;
        }
    }

    return ok;
}

bool DBHandler::updateObjectData(QVector<Object*> objects, QString* error)
{
    bool ok = true;
    QSqlQuery qry;
    QString statement;

    for(int objectIndex = 0; objectIndex < objects.count(); objectIndex++) {
        for(int fieldIndex = 0; fieldIndex < objects.at(objectIndex)->countFields(); fieldIndex++) {
            statement = "UPDATE tbl_objectdata"
                    " SET data='" + objects.at(objectIndex)->getField(fieldIndex)->getData() + "'"
                    " WHERE fk_object=(SELECT id  FROM tbl_objects WHERE barcode='" + objects.at(objectIndex)->getBarcode() + "')"
                    " AND"
                    " fk_datafield=(SELECT id FROM tbl_datafields WHERE name='" + objects.at(objectIndex)->getField(fieldIndex)->getName() + "');";

            if(!this->execute(statement, &qry, error)) {
                ok = false;
            }
        }
    }

    return ok;
}

bool DBHandler::deleteObjects(QVector<QString> barcodes, QString *error)
{
    bool ok = true;
    QSqlQuery qry;
    QVector<int> objectIds;
    QString strObjectIds;
    QString getObjectIds = "SELECT id FROM tbl_objects WHERE barcode IN ";
    QString deleteRentalAssociation;
    QString deleteObjectData;
    QString deleteObjectEntries;

    getObjectIds += "(";
    for(int i = 0; i < barcodes.count(); i++) {
        getObjectIds += "'" + barcodes.at(i) + "'";
        if(i < barcodes.count() -1) {
            getObjectIds += ", ";
        }
    }
    getObjectIds += ")";


    qDebug() << getObjectIds;
    if(!this->execute(getObjectIds, &qry, error)) {
        ok = false;
    }
    else {
        while(qry.next()) {
            objectIds.append(qry.value(0).toInt());
        }
        qry.finish();
    }

    strObjectIds = "(";
    for(int i = 0; i < objectIds.count(); i++) {
        strObjectIds += QString::number(objectIds.at(i));
        if(i < objectIds.count() -1) {
            strObjectIds += ", ";
        }
    }
    strObjectIds += ")";

    deleteRentalAssociation = "DELETE FROM tbl_rental_object WHERE fk_object IN " + strObjectIds;
    deleteObjectData        = "DELETE FROM tbl_objectdata WHERE fk_object IN " + strObjectIds;
    deleteObjectEntries     = "DELETE FROM tbl_objects WHERE id IN " + strObjectIds;

    if(!this->execute(deleteRentalAssociation, &qry, error)) {
        ok = false;
    }
    else if(!this->execute(deleteObjectData, &qry, error)) {
        ok = false;
    }
    else if(!this->execute(deleteObjectEntries, &qry, error)) {
        ok = false;
    }

    return ok;
}

bool DBHandler::insertObjectDataForFieldCreation(QString category, QString fieldname, QString* error)
{
    bool ok = true;
    QSqlQuery qry;
    QVector<int> ids;
    QString statementGetObjectIds = "SELECT id FROM tbl_objects WHERE fk_category=(SELECT id FROM tbl_categories WHERE name='" + category + "');";
    QString insertStatement = "INSERT INTO tbl_objectdata(fk_datafield, fk_object) VALUES";
    if(!this->execute(statementGetObjectIds, &qry, error)) {
        ok = false;
    }
    else {
        while(qry.next()) {
            ids.append(qry.value(0).toInt());
        }
        qry.finish();

        for(int i = 0; i <  ids.count(); i++) {
            insertStatement += "((SELECT id FROM tbl_datafields WHERE name='" + fieldname + "')," + QString::number(ids.at(i)) + ")";
            if(i < ids.count() - 1) {
                insertStatement += ',';
            }
            else {
                insertStatement += ";";
            }
        }

        if(ids.count() == 0) {
            ok = true;
        }
        else if(this->execute(insertStatement, &qry, error)) {
            ok = true;
        }
        else {
            ok = false;
        }
    }

    return ok;
}
