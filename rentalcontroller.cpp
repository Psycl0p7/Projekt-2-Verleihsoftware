#include "rentalcontroller.h"

RentalController::RentalController(DBHandler* dbHandler)
{
    this->dbHandler = dbHandler;
}

void RentalController::searchEntryByBarcode(QString barcode)
{
    QString error;
    QString category;
    QVector<QString> fieldnames;
    QVector<QString> data;
    QVector<Datafield*> fields;
    Entry* entry = NULL;


    if(!this->dbHandler->getFieldnamesByBarcode(barcode, &fieldnames, &error)) {
        qDebug() << error;
    }
    else if(!this->dbHandler->getEntryDataByBarcode(barcode, &category, &data, &error)) {
        qDebug() << error;
    }
    else {
        entry = new Entry(category);
        for(int i = 0; i < fieldnames.count(); i++) {
           entry->addField(new Datafield(fieldnames.at(i), data.at(i)));
        }
    }
    this->entries.append(entry);
    emit this->showRentalEntries(this->entries);
}
