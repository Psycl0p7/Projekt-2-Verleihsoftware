#ifndef OBJECTCONTROLLER_H
#define OBJECTCONTROLLER_H

#include <QObject>
#include <QTableWidgetItem>
#include "object.h"
#include "dbhandler.h"
#include "dialogcontroller.h"

class ObjectController : public QObject
{
    Q_OBJECT
public:
    ObjectController(DBHandler* dbHandler, DialogController* dialogControler);

    void createObject();
    void removeObject(int index);
    void saveDifToDB();
    void setSelectedCategory(int index);
    void searchObjectsByCategory(int categoryIndex);
    void updateToDatabase();

public slots:
    void receiveCategories(QVector<Object*> categories);
    void updateObject(QTableWidgetItem* changedItem);
    void objectChanged(QTableWidgetItem* changedItem);
    void createObject(QString barcode);
signals:
    void updateCateoryList();
    void addObjectToTable(Object* object);
    void showObjects(QVector<Object*> objects);
    void resetTable(QVector<Datafield*> datafields);
private:
    DBHandler* dbHandler;
    int selectedCategoryIndex;
    DialogController* dialogController;
    QVector<Object*> categories;

    QVector<Object*> displayedObjects;
    QVector<Object*> createdObjects;
    QVector<Object*> updatedObjects;
    QVector<Object*> removedObjects;

};

#endif // OBJECTCONTROLLER_H
