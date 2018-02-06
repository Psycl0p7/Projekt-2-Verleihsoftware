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
    static const QString LIST_ALL_CATEGORIES;
    static const int LIST_ALL_INDEX = 0;


    void createObject();
    void removeObject(int index);
    void saveDifToDB();
    void setSelectedCategory(int index);

public slots:
    void receiveCategories(QVector<Object*> categories);
signals:
    void updateCateoryList();

private:
    DBHandler* dbHandler;
    int selectedCategoryIndex;
    DialogController* dialogController;
    QVector<Object*> categories;
    QVector<Object*> currentObjects;
};

#endif // OBJECTCONTROLLER_H
