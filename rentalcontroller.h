#ifndef RENTALCONTROLLER_H
#define RENTALCONTROLLER_H

#include "dbhandler.h"
#include "entry.h"
#include <QObject>

class RentalController : public QObject
{
    Q_OBJECT
public:
    RentalController(DBHandler* dbHandler);

    void searchEntryByBarcode(QString barcode);
private:
    DBHandler* dbHandler;
    QVector<Entry*> entries;
signals:
    void showRentalEntries(QVector<Entry*>);
};

#endif // RENTALCONTROLLER_H
