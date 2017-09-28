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
private:

    QSqlDatabase db;

    bool openDB();
    bool closeDB();
    bool createDB();
    bool DBExists();
};

#endif // DBHANDLER_H
