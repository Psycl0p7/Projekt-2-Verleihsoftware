#include "rentalcontroller.h"

RentalController::RentalController(DBHandler* dbHandler, DialogController *dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
}

void RentalController::tryAddObjectByBarcode(QString barcode)
{
    QString error;
    Object* object = NULL;
    bool objectIsAvailable = false;

    if(this->activeRental->includesObject(barcode)) {
        emit this->dialogController->showInformation("Eintrag ist bereits gelistet.");
    }
    else if(!this->dbHandler->checkObjectAvailability(barcode, &objectIsAvailable, &error)) {
        emit this->dialogController->showWarning("Objektverfügbarkeit konnte nicht geprüft werden", error);
    }
    else if(!objectIsAvailable){
        emit this->dialogController->showInformation("Objekt bereits verliehen.");
    }
    else {
        object = this->searchObjectByBarcode(barcode);
        if(object != NULL) {
            this->activeRental->addObject(object);
            emit this->addRentalObject(object->getCategory());
            emit this->setSelectedObjectIndex(this->activeRental->countObjects() - 1);
        }
    }
}

Object* RentalController::searchObjectByBarcode(QString barcode)
{
    QString error;
    bool found = false;
    Object* object = new Object();

    if(!this->dbHandler->getObjectByBarcode(barcode, object, &found, &error)) {
        object = NULL;
        this->dialogController->showWarning("Barcodesuche konte nicht durchgeführt werden", error);
    }
    else if(!found) {
        object = NULL;
        this->dialogController->showInformation("Kein Eintrag gefunden.");
    }

    return object;
}

void RentalController::switchSelectedObject(int index)
{
    if(index > -1 && index < this->activeRental->countObjects()) {
        emit this->updateObjectDataTable(this->activeRental->getObject(index)->getAllFields());
    }
}

void RentalController::removeSelectedObject(int index)
{
    if(index > -1 && index < this->activeRental->countObjects()) {
        this->activeRental->getObject(index);
        this->activeRental->removeObject(index);

        emit this->showRentalEntries(this->activeRental->getAllObjects());

        if(this->activeRental->countObjects() < 1) {
            // clear with empty vector
            emit this->updateObjectDataTable(QVector<Datafield*>());
        }
        else {
            emit this->setSelectedObjectIndex(index - 1);
        }
    }
}

void RentalController::updateObjectDataTable(QVector<Datafield *> fields)
{
    emit this->adjustObjectDataTableRows(fields.count());
    emit this->showSelectedObjectData(fields);
}

void RentalController::init()
{
    this->activeRental = new Rental();
    this->updateObjectDataTable(QVector<Datafield*>());
    emit this->showRentalEntries(QVector<Object*>());
}

void RentalController::confirmActiveRental(QString firstname, QString lastname, QString extra, QDateTime start, QDateTime end)
{
    QString error = NULL;
    qint64 startSecs = start.toSecsSinceEpoch();
    qint64 endSecs = end.toSecsSinceEpoch();

    if(endSecs - startSecs <= 0) {
        emit this->dialogController->showInformation("Ungültige Zeitspanne");
    }
    else {
        if(!this->dbHandler->createRental(new Rental(firstname, lastname, extra, start, end, this->activeRental->getAllObjects()), &error)) {
            this->dialogController->showWarning("Verleih konnte nicht angelegt werden", error);
        }
        else {
            emit this->dialogController->showInformation("Verleih wurde eingetragen.");
            emit this->resetRentalView();
        }
    }
}

QVector<Rental*> RentalController::getAllLentDevice()
{
    QSqlQuery sql;
    QString error;
    dbHandler->getAllLents(&sql, &error);
    QVector<Rental*> list;
    while(sql.next())
    {
        /*
         * 0 = firstname
         * 1 = lastname
         * 2 = extra
         * 3 = start
         * 4 = end
         */
        Rental* rent = new Rental(sql.value(0).toString(),
                                  sql.value(1).toString(),
                                  sql.value(2).toString(),
                                  QDateTime::fromString(sql.value(3).toString(), "dd.MM.yyyy HH:mm"),
                                  QDateTime::fromString(sql.value(4).toString(), "dd.MM.yyyy HH:mm"),
                                  sql.value(5).toString());
        list.append(rent);
    }
    return list;
}
