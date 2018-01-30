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
        QString tblCategories   = "CREATE TABLE 'tbl_categories' ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL UNIQUE )";
        QString tblDatafields   = "CREATE TABLE 'tbl_datafields' ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL, 'fk_category' INTEGER NOT NULL, 'fk_datatype' INTEGER NOT NULL, 'required' BOOLEAN NOT NULL)";
        QString tblEntrydata  = "CREATE TABLE 'tbl_entrydata'  ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'fk_datafield' INTEGER, 'fk_entry' INTEGER NOT NULL, 'data' TEXT)";
        QString tblDatatypes = "CREATE TABLE 'tbl_datatypes'  ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'name' TEXT);";
        QString tblEntries       = "CREATE TABLE 'tbl_entries'    ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,'fk_category' INTEGER);";

        this->db = QSqlDatabase::addDatabase("QSQLITE");
        this->db.setDatabaseName(path);
        this->db.open();

        QSqlQuery query;

        query.exec(tblCategories);
        query.exec(tblDatafields);
        query.exec(tblEntrydata);
        query.exec(tblDatatypes);
        query.exec(tblEntries);

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

    if(this->execute(statement, &qry, error)) {
        ok = true;
        if(qry.first())
            *customfieldExists = true;
    }
    return ok;
}

bool DBHandler::getCustomfields(QSqlQuery* p_qry, QString *error, QString gereateTyp)
{
    QString statement = QString("SELECT name, fk_datatype FROM tbl_datafields WHERE fk_category=") + "(SELECT id FROM tbl_categories WHERE name='" + gereateTyp + "');";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::createCustomField(QString *error, QString name, QString geraeteTyp, QString datentyp, bool required)
{
    QString statement = "INSERT INTO tbl_datafields (name, fk_category,fk_datatype, required) VALUES('"
            + name + "',"
            + "(SELECT id FROM tbl_categories WHERE name='" + geraeteTyp + "'),"
            + "(SELECT id FROM tbl_datatypes WHERE name='" + datentyp + "'),"
            + QString::number(required)
            + ");";

    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::readCustomField(QString geraetetyp, QString fieldname, QString *name, QString* datatype, bool* required) // commit
{
    bool ok = false;
    QSqlQuery qry;
    QString error;
    QString statement = "SELECT name, required, (SELECT name FROM tbl_datatypes WHERE id=fk_datatype) FROM tbl_datafields WHERE name='"
            + fieldname + "' AND fk_category=(SELECT id FROM tbl_categories WHERE name='" + geraetetyp + "');";

    if(this->execute(statement, &qry, &error)) {
        qry.first();
        *name = qry.value(0).toString();
        *required = qry.value(1).toBool();
        *datatype = qry.value(2).toString();
        ok = true;
    }

    return ok;
}

bool DBHandler::updateCustomField(QString category, QString fieldname, QString newName, QString newDatatype, bool newRequired, QString *error)
{
    QString statement = "UPDATE tbl_datafields SET name='"
            + newName + "',"
            + "fk_datatype=(SELECT id FROM tbl_datatypes WHERE name='" + newDatatype + "'),"
            + "required=" + QString::number(newRequired)
            + " WHERE fk_category=(SELECT id FROM tbl_categories WHERE name='" + category
            + "') AND name='" + fieldname + "';";

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
