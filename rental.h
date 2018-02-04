#ifndef RENTAL_H
#define RENTAL_H

#include <QDateTime>
#include "entry.h"

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
    Entry* getEntry(int index);
    void addEntry(Entry* entry);
    bool removeEntry(int index);
private:
    QVector<Entry*> entries;

    QString firstname;
    QString lastname;
    QString extra;

    QDateTime start;
    QDateTime end;
};

#endif // RENTAL_H
