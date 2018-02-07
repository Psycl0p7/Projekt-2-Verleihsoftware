#include "objectcontroller.h"
#include "dialogcontroller.h"

ObjectController::ObjectController(DBHandler* dbHandler, DialogController* dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
}

// transmitted by settingscontroller
void ObjectController::receiveCategories(QVector<Object *> categories)
{
    this->categories = categories;
}

void ObjectController::setSelectedCategory(int index)
{
    this->selectedCategoryIndex = index;
    emit this->resetTable(this->categories.at(index)->getAllFields());
    this->searchObjectsByCategory(index);
}

void ObjectController::createObject(QString barcode)
{
    Object* object = NULL;
    Object* category = this->categories.at(this->selectedCategoryIndex);

    object = new Object(category->getCategory(), barcode);
    for(int i = 0; i < category->countFields(); i++) {
       object->addField(new Datafield(category->getField(i)->getName(), category->getField(i)->getType(), category->getField(i)->isRequired()));
    }

    this->createdObjects.append(object);
    this->displayedObjects.append(object);
    emit this->showObjects(this->displayedObjects );
}

void ObjectController::searchObjectsByCategory(int categoryIndex)
{
    QVector<Object*> foundObjects;
    QString error;

    if(!this->dbHandler->searchObjectsByCategory(this->categories.at(categoryIndex)->getCategory(), &foundObjects, &error)) {
        emit this->dialogController->showWarning("Objeckte konnten nicht gesucht werden", error);
    }
    else {
        this->displayedObjects  = foundObjects;
        emit this->showObjects(this->displayedObjects );
    }
}

// updated depending on diffs in table intio databse
void ObjectController::updateToDatabase()
{
    QString error;
    if(!this->dbHandler->createObjects(this->createdObjects, &error)) {
        emit this->dialogController->showWarning("Fehler bei Erstllung neuer Objekte", error);
    }

    // update objects
}

void ObjectController::updateObject(QTableWidgetItem* changedItem)
{
    int objectIndex = 1;
    int fieldIndex = -1;
    QString data = changedItem->text();
    if(!data.isEmpty()) {
        objectIndex = changedItem->row();
        fieldIndex = changedItem->column();
        this->displayedObjects.at(objectIndex)->getField(fieldIndex)->setData(data);
    }
}

void ObjectController::objectChanged(QTableWidgetItem *changedItem)
{

}

void ObjectController::removeObject(int index)
{

}
