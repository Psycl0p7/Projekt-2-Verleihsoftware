#ifndef RENTALCONTROLLER_H
#define RENTALCONTROLLER_H

#include "dbhandler.h"
#include "entry.h"
#include <QObject>
#include "dialogcontroller.h"

class RentalController : public QObject
{
    Q_OBJECT
public:
    RentalController(DBHandler* dbHandler, DialogController* dialogController);

    void tryAddEntryByBarcode(QString barcode);
    void switchSelectedEntry(int index);
    void removeSelectedEntry(int index);
private:
    DBHandler* dbHandler;
    DialogController* dialogController;
    QVector<QString> activeEntryBarcodes;
    QVector<Entry*> activeEntries;
    Entry* searchEntryByBarcode(QString barcode);
signals:
    void showRentalEntries(QVector<Entry*>);
    void addRentalEntry(QString entry);
    void showSelectedEntryData(QVector<Datafield*>);
    void setSelectedEntryIndex(int);
};

#endif // RENTALCONTROLLER_H
