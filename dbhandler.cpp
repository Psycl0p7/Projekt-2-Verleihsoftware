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
        QString tblGeraetetyp   = "CREATE TABLE 'tbl_geraetetypen'  ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL UNIQUE )";
        QString tblCustomFeld   = "CREATE TABLE 'tbl_customfelder'  ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' TEXT NOT NULL, 'fk_geraetetyp' INTEGER NOT NULL, 'fk_feldDatentyp' INTEGER NOT NULL, 'pflichtfeld' BOOLEAN NOT NULL)";
        QString tblGeraeteData  = "CREATE TABLE 'tbl_geraeteData'   ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'fk_customfeld' INTEGER, 'fk_geraet' INTEGER NOT NULL, 'data' TEXT)";
        QString tblFelddatentyp = "CREATE TABLE 'tbl_feldDatentypen'('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'name' TEXT);";
        QString tblGeraet       = "CREATE TABLE 'tbl_geraete'       ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,'fk_geraetetyp INTEGER);";

        this->db = QSqlDatabase::addDatabase("QSQLITE");
        this->db.setDatabaseName(path);
        this->db.open();

        QSqlQuery query;

        query.exec(tblGeraetetyp);
        query.exec(tblCustomFeld);
        query.exec(tblGeraeteData);
        query.exec(tblGeraet);
        query.exec(tblFelddatentyp);

        QString addDatentypText = "INSERT INTO tbl_feldDatentypen (name) VALUES('Text')";
        QString addDatentypBool = "INSERT INTO tbl_feldDatentypen (name) VALUES('Boolean')";
        QString addDatentypPDF  = "INSERT INTO tbl_feldDatentypen (name) VALUES('PDF')";
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
        this->db.open();

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
    QString statement = "SELECT name FROM tbl_feldDatentypen ORDER BY name ASC";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::getCategories(QSqlQuery* p_qry, QString* error)
{
    QString statement = "SELECT name FROM tbl_geraeteTypen ORDER BY name ASC";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::createCategory(QString name,QString* error)
{
    QString statement = "INSERT INTO tbl_geraeteTypen (name) VALUES ('" + name + "')";
    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::getCustomfields(QSqlQuery* p_qry, QString *error, QString gereateTyp)
{
    QString statement = QString("SELECT name FROM tbl_customfelder WHERE fk_geraetetyp=") + "(SELECT id FROM tbl_geraetetypen WHERE name='" + gereateTyp + "');";
    return this->execute(statement, p_qry, error);
}

bool DBHandler::createCustomField(QString *error, QString name, QString geraeteTyp, QString datentyp, bool pflichtfeld)
{
    QString statement = "INSERT INTO tbl_customfelder (name, fk_geraetetyp,fk_feldDatentyp, pflichtfeld) VALUES('"
            + name + "',"
            + "(SELECT id FROM tbl_geraetetypen WHERE name='" + geraeteTyp + "'),"
            + "(SELECT id FROM tbl_feldDatentypen WHERE name='" + datentyp + "'),"
            + QString::number(pflichtfeld)
            + ");";
    qDebug() << statement;
    return this->execute(statement, new QSqlQuery(), error);
}

bool DBHandler::readCustomField(QString geraetetyp, QString *name, QString* datentyp, bool* pflichtfeld)
{
    QSqlQuery qry;
    QString error;
    QString statement = "SELECT name, pflichtfeld, fk_feldDatentyp FROM tbl_customfelder WHERE name='"
            + *name + "' AND fk_geraetetyp=(SELECT id FROM tbl_geraetetypen WHERE name='" + geraetetyp + "');";
    qDebug() <<  statement;
    int fk_datentyp = NULL;
    bool ok = false;
    if(this->execute(statement, &qry, &error)) {
        *name = qry.value(0).toString();
        *pflichtfeld = qry.value(1).toBool();
        fk_datentyp = qry.value(2).toInt();
        statement = "SELECT name FROM tbl_feldDatentypen WHERE id=" + QString::number(fk_datentyp);
        qDebug() <<  statement;
        if(this->execute(statement, &qry, &error)) {
            ok = true;
            *datentyp = qry.value(0).toString();
        }
    }

    return ok;
}

bool DBHandler::saveCustomField(QString name, QString gerateTyp,QString datentyp, bool pflichtfeld, QString *error)
{
    QString statement = "UPDATE tbl_customfelder SET name='"
            + name + "',"
            + "fk_feldDatentyp=(SELECT id FROM tbl_feldDatentypen WHERE name='" + datentyp + "'),"
            + "pflichtfeld=" + QString::number(pflichtfeld)
            + " WHERE fk_geraetetyp=(SELECT id FROM tbl_geraetetypen WHERE name='" + gerateTyp
            + "') AND name='" + name + "'";
    qDebug() <<  statement;
    return this->execute(statement, new QSqlQuery(), error);
}
