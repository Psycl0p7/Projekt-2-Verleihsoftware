#include "rentalcontroller.h"

RentalController::RentalController(DBHandler* dbHandler, DialogController *dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
}

void RentalController::tryAddEntryByBarcode(QString barcode)
{
    Entry* entry = NULL;

    if(this->activeEntryBarcodes.indexOf(barcode) > -1) {
        emit this->dialogController->showInformation("Eintrag ist bereits gelistet.");
    }
    else {
        entry = this->searchEntryByBarcode(barcode);
        if(entry != NULL) {
            this->activeEntries.append(entry);
            this->activeEntryBarcodes.append(entry->getBarcode());
            emit this->addRentalEntry(entry->getCategory());
            emit this->setSelectedEntryIndex(this->activeEntries.count() - 1);
        }
    }
}

Entry* RentalController::searchEntryByBarcode(QString barcode)
{
    QString error;
    bool found = false;
    Entry* entry = new Entry();

    if(!this->dbHandler->getEntrybyBarcode(barcode, entry, &found, &error)) {
        entry = NULL;
        this->dialogController->showWarning("Barcodesuche konte nicht durchgeführt werden", error);
    }
    else if(!found) {
        entry = NULL;
        this->dialogController->showInformation("Kein Eintrag gefunden.");
    }

    return entry;
}

void RentalController::switchSelectedEntry(int index)
{
    if(index > -1 && index < this->activeEntries.length()) {
        emit this->updateEntryDataTable(this->activeEntries.at(index)->getAllFields());
    }
}

void RentalController::removeSelectedEntry(int index)
{
    if(index > -1 && index < this->activeEntries.count()) {
        delete this->activeEntries.at(index);
        this->activeEntries.removeAt(index);
        this->activeEntryBarcodes.removeAt(index);

        emit this->showRentalEntries(this->activeEntries);

        if(this->activeEntries.count() < 1) {
            // clear with empty vector
            emit this->updateEntryDataTable(QVector<Datafield*>());
        }
        else {
            emit this->setSelectedEntryIndex(index - 1);
        }
    }
}

void RentalController::updateEntryDataTable(QVector<Datafield *> fields)
{
    emit this->adjustEntryDataTableRows(fields.count());
    emit this->showSelectedEntryData(fields);
}

void RentalController::init()
{
    this->activeEntries.clear();
    this->activeEntryBarcodes.clear();
    this->updateEntryDataTable(QVector<Datafield*>());
    emit this->showRentalEntries(QVector<Entry*>());
}
