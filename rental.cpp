#include "rental.h"

Rental::Rental(QString firstname, QString lastname, QString extra, QDateTime start, QDateTime end)
{
    this->firstname = firstname;
    this->lastname = lastname;
    this->extra = extra;
    this->start = start;
    this->end = end;
}

QString Rental::getFirstname()
{
    return this->firstname;
}

QString Rental::getLastname()
{
    return this->lastname;
}

QString Rental::getExtra()
{
    return this->extra;
}

QDateTime Rental::getStart()
{
    return this->start;
}

QDateTime Rental::getEnd()
{
    return this->end;
}

// negative values if time expired
qint64 Rental::getTimeLeft()
{
    qint64 startSecs = this->start.toSecsSinceEpoch();
    qint64 endSecs = this->end.toSecsSinceEpoch();
    qint64 secondsLeft = endSecs - startSecs;

    return secondsLeft;
}

int Rental::countObjects()
{
    return this->objects.count();
}

Object* Rental::getObject(int index)
{
    Object* object = NULL;
    if(index < this->objects.count()) {
        object = this->objects.at(index);
    }
    return object;
}

void Rental::addObject(Object* object)
{
    this->objects.append(object);
}

bool Rental::removeObject(int index)
{
    bool ok = false;
    if(index < this->objects.count()) {
        this->objects.removeAt(index);
        ok = true;
    }
    return ok;
}

bool Rental::includesObject(QString barcode)
{
    bool includesIt = false;
}

QVector<Object*> Rental::getAllObjects()
{
    return this->objects;
}
