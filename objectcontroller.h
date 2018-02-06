#ifndef OBJECTCONTROLLER_H
#define OBJECTCONTROLLER_H

#include <QObject>
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

public slots:
    void receiveCategories(QVector<Object*> categories);
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
    QVector<Object*> currentObjects;
    QVector<Object*> createdObjects;
    QVector<Object*> updatedObjects;
};

#endif // OBJECTCONTROLLER_H
