#ifndef RENTAL_H
#define RENTAL_H

#include <QDateTime>
#include "object.h"

class Rental
{
public:
    Rental(QString firstname, QString lastname, QString extra, QDateTime start, QDateTime end);
    QString getFirstname();
    QString getLastname();
    QString getExtra();
    QDateTime getStart();
    QDateTime getEnd();
    qint64 getTimeLeft();

    int countEntries();
    Object* getObject(int index);
    void addObject(Object* object);
    bool removeObject(int index);
private:
    QVector<Object*> entries;

    QString firstname;
    QString lastname;
    QString extra;

    QDateTime start;
    QDateTime end;
};

#endif // RENTAL_H
