#include "rentalcontroller.h"

RentalController::RentalController(DBHandler* dbHandler, DialogController *dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
}

void RentalController::tryAddObjectByBarcode(QString barcode)
{
    Object* object = NULL;

    if(this->activeRental->includesObject(barcode)) {
        emit this->dialogController->showInformation("Eintrag ist bereits gelistet.");
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
    this->activeRental = NULL;
    this->updateObjectDataTable(QVector<Datafield*>());
    emit this->showRentalEntries(QVector<Object*>());
}

void RentalController::confirmActiveRental(QString firstname, QString lastname, QString extra, QDateTime start, QDateTime end, QDateTime now)
{
    qint64 nowSecs = now.toSecsSinceEpoch();
    qint64 startSecs = start.toSecsSinceEpoch();
    qint64 endSecs = end.toSecsSinceEpoch();

    if(endSecs - startSecs < 0 || startSecs < nowSecs) {
        emit this->dialogController->showInformation("Ungültige Zeitspanne");
    }
    else {
        // db handler create Rental
    }
}
