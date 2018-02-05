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
 * tbl_geraeteData    -> tbl_entrydata
 * tbl_feldDatentypen -> tbl_datatypes
 * tbl_geraete        -> tbl_entries
 */

bool DBHandler::createDB()
{
    bool dbCreated = false;
    if(!DBExists()) {
        QString path = "db.sqlite";
        QString tblCategories = "CREATE TABLE 'tbl_categories' ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL UNIQUE )";
        QString tblDatafields = "CREATE TABLE 'tbl_datafields' ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL, 'fk_category' INTEGER NOT NULL, 'fk_datatype' INTEGER NOT NULL, 'required' BOOLEAN NOT NULL)";
        QString tblEntrydata  = "CREATE TABLE 'tbl_entrydata'  ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'fk_datafield' INTEGER, 'fk_entry' INTEGER NOT NULL, 'data' TEXT)";
        QString tblDatatypes  = "CREATE TABLE 'tbl_datatypes'  ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'name' TEXT);";
        QString tblEntries    = "CREATE TABLE 'tbl_entries'    ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'fk_category' INTEGER, 'barcode' TEXT NOT NULL UNIQUE);";
        QString tblRentals    = "CREATE TABLE 'tbl_rentals'    ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'firstname' TEXT NOT NULL, 'lastname' TEXT NOT NULL, 'extra' TEXT, 'start' TEXT, end TEXT);";


        this->db = QSqlDatabase::addDatabase("QSQLITE");
        this->db.setDatabaseName(path);
        this->db.open();

        QSqlQuery query;

        query.exec(tblCategories);
        query.exec(tblDatafields);
        query.exec(tblEntrydata);
        query.exec(tblDatatypes);
        query.exec(tblEntries);
        query.exec(tblRentals);

        QString addDatentypText = "INSERT INTO tbl_datatypes (name) VALUES('Text')";

        query.exec(addDatentypText);
        db.close();

        dbCreated = true;
    }
    if(DBExists()) {
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
    QString statement = "SELECT name FROM tbl_categories";
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
    //@TODO löschen aller Geräteeintrage sowie GeräteDaten
    QString statement = "DELETE FROM tbl_categories WHERE name='"
            + name
            + "';";
    return this->execute(statement, new QSqlQuery(), error);
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

    qDebug() << statement;

    if(this->execute(statement, &qry, error)) {
        ok = true;
        if(qry.first())
            *customfieldExists = true;
    }
    return ok;
}

bool DBHandler::getCustomfields(QSqlQuery* p_qry, QString *error, QString gereateTyp)
{
    QString statement = QString("SELECT name, fk_datatype FROM tbl_datafields WHERE fk_category="
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
    qDebug() << "statement: " << statement;

    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::readCustomField(QString* error, QString category, QString fieldname, int *datatype, bool* required)
{
    bool ok = false;
    QSqlQuery qry;
    QString statement = "SELECT fk_datatype,required FROM tbl_datafields WHERE name='"
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
    qDebug() << statement;
    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::deleteCustomField(QString category, QString fieldname, QString* error)
{
    QString statement = "DELETE FROM tbl_datafields WHERE name='"
            + fieldname
            + "' AND fk_category=(SELECT id FROM tbl_categories WHERE name='"
            + category
            + "');";

    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::getEntrybyBarcode(QString barcode, Entry* entry, bool *found, QString* error)
{
    bool ok = true;
    QSqlQuery qry;

    // entry construction data
    QString category;

    // fetch in same order to allaw synchronious merging
    QVector<QString> fieldnames;
    QVector<QString> data;

    QString statementGetFieldnames = "SELECT"
            " tbl_datafields.name"
            " FROM tbl_datafields"
            " WHERE tbl_datafields.fk_category = (SELECT tbl_entries.fk_category FROM tbl_entries WHERE tbl_entries.barcode = '" + barcode +"')"
            " ORDER BY tbl_datafields.id ASC;";

    QString statementGetData = "SELECT"
            " tbl_categories.name AS category,"
            " tbl_entrydata.data"
            " FROM tbl_entries"
            " INNER JOIN tbl_categories"
            " ON tbl_categories.id = tbl_entries.fk_category"
            " INNER JOIN tbl_entrydata"
            " ON tbl_entrydata.fk_entry = tbl_entries.id"
            " WHERE barcode = '" + barcode + "'"
            " ORDER BY tbl_entrydata.fk_datafield ASC;";

    *found = false;

    if(!this->execute(statementGetFieldnames, &qry, error)) {
        ok = false;
    }
    else if(qry.first()) {
        *found = true;

        fieldnames.append(qry.value(0).toString());
        while(qry.next()) {
            fieldnames.append(qry.value(0).toString());
        }

        qry.finish();

        if(!this->execute(statementGetData, &qry, error)) {
            ok = false;
        }
        else {
            // get category in first record
            if(qry.first()) {
                category = qry.value(0).toString();
                data.append(qry.value(1).toString());
            }

            // fetch rest of data
            while(qry.next()) {
                data.append(qry.value(1).toString());
            }

            if(fieldnames.length() != data.length()) {
                ok = false;
                *error = "invalid entry data";
            }
            else {
                // create Entry object
                entry->setBarcode(barcode);
                entry->setCategory(category);
                for(int i = 0; i < fieldnames.length(); i++) {
                    entry->addField(new Datafield(fieldnames.at(i), data.at(i)));
                }
            }
        }
    }

    return ok;
}


/** Holt sich alle erstellten Typen von Geräte aus der Datenbank. Für die Visuelle Darstellung
 * @brief DBHandler::getAllDeviceTypes
 * @param p_qry - Enthält den Query für die SQL Abfrage
 * @param error - Enthält bei einem Fehler, die Fehlermeldung
 * @return SqlQuery - Enthält alle Device Typen aus der DB
 */
bool DBHandler::getAllDeviceTypes(QSqlQuery* p_qry, QString* error)
{
    QString statement = QString("SELECT name FROM tbl_datafields WHERE category = (SELECT name FROM tbl_categories)");

    return this->execute(statement, p_qry, error);
}

/**
 * Sucht anhand der ID/BarCode einen Daten und Updatet diesen oder erstellt einen neuen, wenn er nicht gefunden worden ist
 * @brief DBHandler::findAndUpdateDevice
 * @param id - Enthält die ID des gesuchten Geräts
 * @param data - ENthält den neuen oder alten Datensatz eines Felds
 * @param field - Enthält das Feld, in dem es gespeichert werden soll
 */
bool DBHandler::findAndUpdateDevice(QSqlQuery* p_qry, QString* error, QString id, QString data, QString field)
{
    QString statement = QString("UPDATE tbl_entrydata SET " + field + " = " + data + " WHERE ID = " + id);
}






/**
 * Speichert neue Geräte in die Datenbank oder Updatet die Felder, wenn diese schon vorhanden sind
 * @brief DBHandler::saveNewDeviceData
 * @param p_qry
 * @param error
 * @param id - Feldname
 * @param data - Zu speichender Wert im Feld
 * @param field - Feldname
 * @return
 */
bool DBHandler::saveNewDeviceDataGetID(QSqlQuery* p_qry, QString* error, QString id, QString data, QString field, QString category)
{
    QString statement = QString("SELECT id from tbl_categories where tbl_categories.name = '" + category + "'");
    return this->execute(statement, p_qry, error);
}

/**
 * @brief DBHandler::saveNewDeviceDataGetNextID
 * @param p_qry
 * @param error
 * @param id
 * @param data
 * @param field
 * @param category
 * @return
 */
bool DBHandler::saveNewDeviceDataGetNextID(QSqlQuery* p_qry, QString* error, QString id, QString data, QString field, QString category)
{
    QString statement = QString("SELECT id FROM tbl_datafields "
                                "WHERE tbl_datafields.fk_category = '" + id + "' AND tbl_datafields.name = '" + field + "'");
    return this->execute(statement, p_qry, error);
}



/**
 * Sucht alle Tabellen mit dem Meta begriff
 * @brief DBHandler::findAllSearchedData
 * @param p_qry
 * @param error
 * @param searchPara - Zu suchender Begriff
 * @return
 */
bool DBHandler::findAllSearchedData(QSqlQuery* p_qry, QString* error, QString searchPara)
{
    QString statement = QString("SELECT * FROM tbl_entrydata WHERE data LIKE '%" + searchPara + "%'");
    return this->execute(statement, p_qry, error);
}

/**
 * @brief DBHandler::existDeviceInDB
 * @param p_qry
 * @param error
 * @param ID
 * @return
 */
bool DBHandler::existDeviceInDB(QSqlQuery* p_qry, QString* error, QString ID)
{
    QString statement = QString("SELECT * FROM tbl_entrydata "
                                "LEFT JOIN tbl_datafields "
                                "ON tbl_entrydata.fk_datafield = tbl_datafields.id WHERE "
                                "tbl_datafields.name = 'Barcode' AND tbl_entrydata.data = '" + ID + "'");
    return this->execute(statement, p_qry, error);
}

/**
 * @brief DBHandler::getAllDevicesForACategory
 * @param p_qry
 * @param error
 * @param cat
 * @return
 */
/*bool DBHandler::getAllDevicesForACategory(QSqlQuery* p_qry, QString* error, QString cat)
{
    QString statement = QString("SELECT tbl_entrydata.fk_entry, tbl_datafields.name, tbl_entrydata.data "
                                   "FROM tbl_datafields, tbl_entrydata, tbl_entries"
                                   "LEFT JOIN tbl_categories ON tbl_entries.id = tbl_entrydata.fk_entry"
                                   "WHERE tbl_datafields.id = tbl_entrydata.fk_datafield AND tbl_categories.name = " + cat +"");
    return this->execute(statement, p_qry, error);
}*/





bool DBHandler::getAllDevicesForACategory(QSqlQuery* p_qry, QString* error, QString field, QString cat)
{
    QString statement = QString("SELECT tbl_entrydata.data, tbl_entrydata.fk_datafield, tbl_entrydata.fk_entry "
                                "FROM tbl_datafields, tbl_entrydata, tbl_entries "
                                "LEFT JOIN tbl_categories ON tbl_entries.id = tbl_entrydata.fk_entry "
                                "WHERE tbl_datafields.id = tbl_entrydata.fk_datafield "
                                "AND tbl_categories.name = '" + cat +"' AND tbl_datafields.name = '" + field + "' "
                                "ORDER BY tbl_entrydata.fk_entry");
    return this->execute(statement, p_qry, error);
}

