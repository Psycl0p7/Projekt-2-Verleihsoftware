#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QFile>
#include <QVariant>
#include <QDateTime>
#include <QCryptographicHash>
#include <QSqlError>

class DBHandler
{
public:
    DBHandler();

    bool geratetypAnlegen(QString name, QString *error);
    bool geratetypenAuslesen(QSqlQuery* p_qry, QString *error);
    bool getCustomFelder(QSqlQuery* p_qry, QString *error, QString gereateTyp);
    bool feldDatentypenAuslesen(QSqlQuery* p_qry, QString *error);

    bool createNewCustomField(QString *error, QString name, QString geraeteTyp, QString datentyp, bool pflichtfeld);
    bool saveCustomField(QString *error);
private:

    QSqlDatabase db;

    bool execute(QString statement, QSqlQuery* p_qry, QString *error);
    bool openDB();
    bool closeDB();
    bool createDB();
    bool DBExists();
};

#endif // DBHANDLER_H
