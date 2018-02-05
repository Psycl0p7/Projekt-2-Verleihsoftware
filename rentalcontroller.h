#ifndef RENTALCONTROLLER_H
#define RENTALCONTROLLER_H

#include "dbhandler.h"
#include "object.h"
#include <QObject>
#include "dialogcontroller.h"

class RentalController : public QObject
{
    Q_OBJECT
public:
    RentalController(DBHandler* dbHandler, DialogController* dialogController);

    void init();
    void tryAddObjectByBarcode(QString barcode);
    void switchSelectedObject(int index);
    void removeSelectedObject(int index);
    void confirmActiveRental();

private:
    DBHandler* dbHandler;
    DialogController* dialogController;
    QVector<QString> activeObjectBarcodes;
    QVector<Object*> activeEntries;
    Object* searchObjectByBarcode(QString barcode);
    void updateObjectDataTable(QVector<Datafield*> fields);
signals:
    void showRentalEntries(QVector<Object*>);
    void addRentalObject(QString object);
    void showSelectedObjectData(QVector<Datafield*>);
    void setSelectedObjectIndex(int);
    void adjustObjectDataTableRows(int);
};

#endif // RENTALCONTROLLER_H
