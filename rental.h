#ifndef RENTAL_H
#define RENTAL_H

#include <QDateTime>
#include "object.h"

class Rental
{
public:
    Rental();
    Rental(QString firstname, QString lastname, QString extra, QDateTime start, QDateTime end);
    Rental(QString firstname, QString lastname, QString extra, QString start, QString end);
    Rental(QString firstname, QString lastname, QString extra, QDateTime start, QDateTime end, QVector<Object*> objects);
    QString getFirstname();
    QString getLastname();
    QString getExtra();
    QDateTime getStart();
    QDateTime getEnd();
    qint64 getTimeLeft();
    bool includesObject(QString barcode);

    int countObjects();
    QVector<Object*> getAllObjects();
    Object* getObject(int index);
    void addObject(Object* object);
    bool removeObject(int index);
private:
    QVector<Object*> objects;

    QString firstname;
    QString lastname;
    QString extra;

    QDateTime start;
    QDateTime end;

    QString startDate;
    QString endDate;
};

#endif // RENTAL_H
