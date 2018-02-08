#include "objectcontroller.h"
#include "dialogcontroller.h"

ObjectController::ObjectController(DBHandler* dbHandler, DialogController* dialogController)
{
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
    this->selectedCategoryIndex = -1;
    this->tableReady = false;
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

void ObjectController::setTableReady(bool isReady)
{
    this->tableReady = isReady;
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
    this->createdObjectsBarcodes.append(barcode);
    this->displayedObjects.append(object);
    emit this->showObjects(this->displayedObjects );
}

void ObjectController::searchObjectsByCategory(int categoryIndex)
{
    QVector<Object*> foundObjects;
    QString error;

    if(!this->dbHandler->searchObjectsByCategory(this->categories.at(categoryIndex)->getCategory(), &foundObjects, &error)) {
        emit this->dialogController->showWarning("Objekte konnten nicht gesucht werden", error);
    }
    else {
        for(int i = 0; i < this->displayedObjects.count(); i++) {
            delete this->displayedObjects.at(i);
        }
        displayedObjects.clear();
        this->displayedObjects  = foundObjects;
        emit this->showObjects(this->displayedObjects);
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
        // create
        if(this->createdObjects.count() > 0) {
            if(!this->dbHandler->createObjects(this->createdObjects, &error)) {
                emit this->dialogController->showWarning("Fehler bei Erstellung neuer Objekte", error);
            }
            else if(!this->dbHandler->insertObjectData(this->createdObjects, &error)) {
                emit this->dialogController->showWarning("Fehler bei der Erstellung von Objektdaten neuer Objekte", error);
            }
        }

        // update
        if(this->updatedObjects.count() > 0) {
            if(!this->dbHandler->updateObjectData(this->updatedObjects, &error)) {
                emit this->dialogController->showWarning("Fehler bei Datenüberschreibung", error);
            }
        }
        // delete
        if(this->removedObjects.count() > 0) {
            if(!this->dbHandler->deleteObjects(this->removedObjects, &error)) {
                emit this->dialogController->showWarning("Fehler bei Löschung von Objekten und zusammenhängenden Daten", error);
            }
        }

        this->createdObjects.clear();
        this->createdObjectsBarcodes.clear();
        this->updatedObjects.clear();

        for(int i = 0; i < removedObjects.count(); i++) {
            delete this->removedObjects.at(i);
        }
        this->removedObjects.clear();
    }
}

void ObjectController::updateObject(QTableWidgetItem* changedItem)
{
    if(this->tableReady) {
        int objectIndex = changedItem->row();
        int fieldIndex = changedItem->column();
        QString data = changedItem->text();
        // update data via pointer (affects createdObjects, updatedObjects)
        this->displayedObjects.at(objectIndex)->getField(fieldIndex)->setData(data);
        // if its not an create obj, update it
        if(this->createdObjectsBarcodes.indexOf(this->displayedObjects.at(objectIndex)->getBarcode()) < 0) {
            this->updatedObjects.append(this->displayedObjects.at(objectIndex));
        }
    }
}

void ObjectController::removeObject(int index)
{
    this->removedObjects.append(this->displayedObjects.at(index));
    this->displayedObjects.removeAt(index);
    emit this->showObjects(this->displayedObjects);
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

void ObjectController::discardChanged()
{
    this->createdObjects.clear();
    this->createdObjectsBarcodes.clear();
    this->updatedObjects.clear();
    this->removedObjects.clear();
    this->searchObjectsByCategory(this->selectedCategoryIndex);
}

