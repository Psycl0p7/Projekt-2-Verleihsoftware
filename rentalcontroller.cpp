#include "rentalcontroller.h"

RentalController::RentalController(DBHandler* dbHandler, DialogController *dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
}

void RentalController::tryAddObjectByBarcode(QString barcode)
{
    Object* object = NULL;

    if(this->activeObjectBarcodes.indexOf(barcode) > -1) {
        emit this->dialogController->showInformation("Eintrag ist bereits gelistet.");
    }
    else {
        object = this->searchObjectByBarcode(barcode);
        if(object != NULL) {
            this->activeEntries.append(object);
            this->activeObjectBarcodes.append(object->getBarcode());
            emit this->addRentalObject(object->getCategory());
            emit this->setSelectedObjectIndex(this->activeEntries.count() - 1);
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
    if(index > -1 && index < this->activeEntries.length()) {
        emit this->updateObjectDataTable(this->activeEntries.at(index)->getAllFields());
    }
}

void RentalController::removeSelectedObject(int index)
{
    if(index > -1 && index < this->activeEntries.count()) {
        delete this->activeEntries.at(index);
        this->activeEntries.removeAt(index);
        this->activeObjectBarcodes.removeAt(index);

        emit this->showRentalEntries(this->activeEntries);

        if(this->activeEntries.count() < 1) {
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
    this->activeEntries.clear();
    this->activeObjectBarcodes.clear();
    this->updateObjectDataTable(QVector<Datafield*>());
    emit this->showRentalEntries(QVector<Object*>());
}
