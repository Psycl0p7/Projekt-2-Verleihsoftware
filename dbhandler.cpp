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
    if(db.exists())
    {
        dbExists = true;
    }

    return dbExists;
}

bool DBHandler::createDB()
{
    if(!DBExists())
    {
        QString path = "db.sqlite";
        QString tblCategories           = "CREATE TABLE 'tbl_categories'  ('id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, 'name' TEXT NOT NULL UNIQUE )";
        QString tblCustomfields         = "CREATE TABLE 'tbl_customfields'  ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL, 'fk_category' INTEGER NOT NULL, 'fk_customfieldDatatype' INTEGER NOT NULL, 'required' BOOLEAN NOT NULL)";
        QString tblDeviceData           = "CREATE TABLE 'tbl_deviceData'   ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'fk_customfield' INTEGER, 'fk_device' INTEGER NOT NULL, 'data' TEXT)";
        QString tblDevice               = "CREATE TABLE 'tbl_devices'       ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,'fk_category INTEGER);";
        QString tblCustomfieldDatatypes = "CREATE TABLE 'tbl_customfieldDatatypes'('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'name' TEXT);";

        this->db = QSqlDatabase::addDatabase("QSQLITE");
        this->db.setDatabaseName(path);
        this->db.open();

        QSqlQuery query;

        query.exec(tblCategories);
        query.exec(tblCustomfields);
        query.exec(tblDeviceData);
        query.exec(tblDevice);
        query.exec(tblCustomfieldDatatypes);

        QString addDatentypText = "INSERT INTO tbl_customfieldDatatypes (name) VALUES('Text')";
        QString addDatentypBool = "INSERT INTO tbl_customfieldDatatypes (name) VALUES('Boolean')";
        QString addDatentypPDF  = "INSERT INTO tbl_customfieldDatatypes (name) VALUES('PDF')";
        query.exec(addDatentypText);
        query.exec(addDatentypBool);
        query.exec(addDatentypPDF);
        db.close();

        return true;
    }
    if(DBExists())
    {
        this->db = QSqlDatabase::addDatabase("QSQLITE");
        this->db.setDatabaseName("db.sqlite");
        return false;
    }
}

bool DBHandler::openDB()
{
    bool isOpen = false;
    db.open();

    if(this->db.isOpen())
        isOpen = true;

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

    if(!this->openDB())
            *error = "Datenbank konnte nicht geöffnet werden.";
    else
    {
        p_qry->prepare(statement);

        if(!p_qry->exec())
            *error = "Datenbankabfrage konnte nicht ausgeführt werden: " + p_qry->lastError().text();
        else
            ok = true;
    }

    return ok;
}

/********************************************************************************
 *                              DB-Anfragen                                     *
 ********************************************************************************/


bool DBHandler::readSupportedDatatypes(QSqlQuery* p_qry, QString *error)
{
    QString statement = "SELECT name FROM tbl_customfieldDatatypes ORDER BY name ASC";
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

bool DBHandler::checkCustomfieldExists(QString fieldName, QString categoryName, bool* customfieldExists,QString *error)
{
    bool ok = false;
    QSqlQuery qry;
    QString statement = "SELECT name FROM tbl_customfields WHERE name='"
            + fieldName
            + "' AND fk_category=(SELECT id FROM tbl_categories WHERE name='"
            + categoryName
            + "');";
    qDebug() << statement;
    if(this->execute(statement, &qry, error))
    {
        ok = true;
        if(qry.first())
            *customfieldExists = true;
    }
    return ok;
}

bool DBHandler::getCustomfields(QSqlQuery* p_qry, QString *error, QString gereateTyp)
{
    QString statement = QString("SELECT name FROM tbl_customfields WHERE fk_category=") + "(SELECT id FROM tbl_categories WHERE name='" + gereateTyp + "');";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::createCustomField(QString *error, QString name, QString geraeteTyp, QString datentyp, bool required)
{
    QString statement = "INSERT INTO tbl_customfields (name, fk_category,fk_customfieldDatatype, required) VALUES('"
            + name + "',"
            + "(SELECT id FROM tbl_categories WHERE name='" + geraeteTyp + "'),"
            + "(SELECT id FROM tbl_customfieldDatatypes WHERE name='" + datentyp + "'),"
            + QString::number(required)
            + ");";
    qDebug() << statement;
    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::readCustomField(QString geraetetyp, QString *name, QString* datentyp, bool* required)
{
    QSqlQuery qry;
    QString error;
    QString statement = "SELECT name, required, fk_customfieldDatatype FROM tbl_customfields WHERE name='"
            + *name + "' AND fk_category=(SELECT id FROM tbl_categories WHERE name='" + geraetetyp + "');";
    qDebug() <<  statement;
    int fk_datentyp = NULL;
    bool ok = false;
    if(this->execute(statement, &qry, &error)) {
        *name = qry.value(0).toString();
        *required = qry.value(1).toBool();
        fk_datentyp = qry.value(2).toInt();
        statement = "SELECT name FROM tbl_customfieldDatatypes WHERE id=" + QString::number(fk_datentyp);
        qDebug() <<  statement;
        if(this->execute(statement, &qry, &error)) {
            ok = true;
            *datentyp = qry.value(0).toString();
        }
    }

    return ok;
}

bool DBHandler::saveCustomField(QString name, QString gerateTyp,QString datentyp, bool required, QString *error)
{
    QString statement = "UPDATE tbl_customfields SET name='"
            + name + "',"
            + "fk_customfieldDatatype=(SELECT id FROM tbl_customfieldDatatypes WHERE name='" + datentyp + "'),"
            + "required=" + QString::number(required)
            + " WHERE fk_category=(SELECT id FROM tbl_categories WHERE name='" + gerateTyp
            + "') AND name='" + name + "'";
    qDebug() <<  statement;
    return this->execute(statement, new QSqlQuery(), error);
}
