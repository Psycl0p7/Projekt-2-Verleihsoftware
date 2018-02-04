#include "rentalcontroller.h"

RentalController::RentalController(DBHandler* dbHandler, DialogController *dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
}

void RentalController::searchEntryByBarcode(QString barcode)
{
    QString error;
    Entry* entry = new Entry();

    if(!this->dbHandler->getEntrybyBarcode(barcode, entry, &error)) {
        this->dialogController->showWarning("Barcodesuche konte nicht durchgeführt werden", error);
    }
    else {
        QString debugBarcode = entry->getBarcode();

        if(this->activeEntryBarcodes.indexOf(debugBarcode) > -1){
            this->dialogController->showInformation("Gerät ist bereits gelistet.");
        }
        else {
            this->activeEntryBarcodes.append(entry->getBarcode());
            this->activeEntries.append(entry);
            emit this->showRentalEntries(this->activeEntries);
        }
    }
}

void RentalController::switchSelectedEntry(int index)
{
    if(index > -1 && index < this->activeEntries.length()) {
        emit this->showSelectedEntryData(this->activeEntries.at(index)->getAllFields());
    }
}
