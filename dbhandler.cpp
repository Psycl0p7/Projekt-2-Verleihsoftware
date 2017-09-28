#include "dbhandler.h"

/********************************************************************************
 *                              DB-Anfragen                                     *
 ********************************************************************************/

bool DBHandler::feldDatentypenAuslesen(QSqlQuery* p_qry, QString *error)
{
    bool ok = false;

    if(!this->openDB())
            *error = "Datenbank konnte nicht geöffnet werden.";
    else
    {
        p_qry->prepare("SELECT name FROM tbl_feldDatentypen ORDER BY name ASC");

        if(!p_qry->exec())
            *error = "Datenbankabfrage konnte nicht ausgeführt werden: " + p_qry->lastError().text();
        else
            ok = true;
    }

    return ok;
}

bool DBHandler::geratetypAnlegen(QString name,QString* error)
{
    bool ok = false;

    if(!this->openDB())
            *error = "Datenbank konnte nicht geöffnet werden.";
    else
    {
        QSqlQuery qry(this->db);
        qry.prepare("INSERT INTO tbl_geraeteTypen (name) VALUES ('" + name + "')");

        if(!qry.exec())
            *error = "Datenbankabfrage konnte nicht ausgeführt werden: " + qry.lastError().text();
        else
            ok = true;
    }

    return ok;
}

bool DBHandler::geratetypenAuslesen(QSqlQuery* p_qry, QString* error)
{
    bool ok = false;

    if(!this->openDB())
            *error = "Datenbank konnte nicht geöffnet werden.";
    else
    {
        p_qry->prepare("SELECT name FROM tbl_geraeteTypen ORDER BY name ASC");

        if(!p_qry->exec())
            *error = "Datenbankabfrage konnte nicht ausgeführt werden: " + p_qry->lastError().text();
        else
            ok = true;
    }

    return ok;
}

bool DBHandler::getCustomFelder(QSqlQuery* p_qry, QString *error, QString gereateTyp)
{
    bool ok = false;

    if(!this->openDB())
            *error = "Datenbank konnte nicht geöffnet werden.";
    else
    {
        p_qry->prepare("SELECT name FROM tbl_customfelder WHERE fk_geraetetyp='" + gereateTyp + "'");

        if(!p_qry->exec())
            *error = "Datenbankabfrage konnte nicht ausgeführt werden: " + p_qry->lastError().text();
        else
            ok = true;
    }

    return ok;
}

/********************************************************************************
 *                              INIT-METHODEN                                   *
 ********************************************************************************/

DBHandler::DBHandler()
{
    createDB();
}

bool DBHandler::createDB()
{
    if(!DBExists())
    {
        QString path = "db.sqlite";
        QString tblGeraetetyp   = "CREATE TABLE 'tbl_geraetetypen'    ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' CHAR NOT NULL UNIQUE )";
        QString tblCustomFeld   = "CREATE TABLE 'tbl_customfelder'   ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'name' CHAR NOT NULL, 'fk_geraetetyp' CHAR NOT NULL, 'fk_feldDatentyp' CHAR NOT NULL, 'pflichtfeld' BOOLEAN NOT NULL)";
        QString tblGeraeteData  = "CREATE TABLE 'tbl_geraeteData'     ('id' INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  UNIQUE, 'fk_customfeld' INTEGER, 'fk_geraet' INTEGER NOT NULL, 'data' CHAR)";
        QString tblFelddatentyp = "CREATE TABLE 'tbl_feldDatentypen' ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, 'name' TEXT);";
        QString tblGeraet       = "CREATE TABLE 'tbl_geraete' ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,'fk_geraetetyp INTEGER);";

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
