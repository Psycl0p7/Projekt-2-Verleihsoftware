#include "objectcontroller.h"
#include "dialogcontroller.h"

const QString ObjectController::LIST_ALL_CATEGORIES = "Alle";

ObjectController::ObjectController(DBHandler* dbHandler, DialogController* dialogController)
{
    this->selectedCategoryIndex = ObjectController::LIST_ALL_INDEX;
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
}

void ObjectController::createObject()
{
    Object* object = NULL;
    if(this->selectedCategoryIndex > ObjectController::LIST_ALL_INDEX) {
        object = new Object(this->categories.at(this->selectedCategoryIndex)->getCategory());
    }
}

void ObjectController::removeObject(int index)
{

}

void ObjectController::saveDifToDB()
{

}
