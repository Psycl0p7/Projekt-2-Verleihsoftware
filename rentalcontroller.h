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

    void searchEntryByBarcode(QString barcode);
    void switchSelectedEntry(int index);
private:
    DBHandler* dbHandler;
    DialogController* dialogController;
    QVector<QString> activeEntryBarcodes;
    QVector<Entry*> activeEntries;
signals:
    void showRentalEntries(QVector<Entry*>);
    void showSelectedEntryData(QVector<Datafield*>);
};

#endif // RENTALCONTROLLER_H
