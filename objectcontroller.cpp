#include "objectcontroller.h"
#include "dialogcontroller.h"

ObjectController::ObjectController(DBHandler* dbHandler, DialogController* dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
    this->selectedCategoryIndex = -1;
}

// transmitted by settingscontroller
void ObjectController::receiveCategories(QVector<Object *> categories)
{
    this->categories = categories;
    if(this->selectedCategoryIndex > -1) {
        emit this->resetTable(this->categories.at(this->selectedCategoryIndex)->getAllFields());
    }
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
    if(!this->checkRequiredData()) {
        emit this->dialogController->showInformation("Es sind nicht alle Pflichtfelder gefüllt.");
    }
    else {
        if(!this->dbHandler->createObjects(this->createdObjects, &error)) {
            emit this->dialogController->showWarning("Fehler bei Erstellung neuer Objekte", error);
        }
        else if(!this->dbHandler->updateObjects(this->displayedObjects, &error)) {
            emit this->dialogController->showWarning("Fehler bei der Speicherung von Objektdaten", error);
        }
    }
}

void ObjectController::updateObject(QTableWidgetItem* changedItem)
{
    int objectIndex = changedItem->row();
    int fieldIndex = changedItem->column();
    QString data = changedItem->text();
    this->displayedObjects.at(objectIndex)->getField(fieldIndex)->setData(data);
}

bool ObjectController::checkRequiredData()
{
    bool ok = true;
    for(int objectIndex = 0; objectIndex < this->displayedObjects.count(); objectIndex++) {
        for(int fieldIndex = 0; fieldIndex < this->displayedObjects.at(objectIndex)->countFields(); fieldIndex++) {
            if(this->displayedObjects.at(objectIndex)->getField(fieldIndex)->isRequired() && this->displayedObjects.at(objectIndex)->getField(fieldIndex)->getData().isEmpty()) {
                ok = false;
            }
        }
    }
    return ok;
}

void ObjectController::removeObject(int index)
{

}
