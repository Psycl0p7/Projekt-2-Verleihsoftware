#include "settings.h"

Settings::Settings(DBHandler* dbHandler)
{
    this->dbHandler = dbHandler;
}

Settings::~Settings()
{

}

void Settings::init()
{
    // this->ui->cb_category->addItem(MainWindow::CREATE_OPERATOR);
    // this->ui->cb_customfield->addItem(MainWindow::CREATE_OPERATOR);
    this->readSupportedDatatypes();
    this->initCategories();
    this->initCustomfields();
}

int Settings::getCategoryIndex(QString name)
{
    int index = -1;
    for(int i = 0; i < this->categories.count(); i++) {
        if(name == this->categories.at(i)->getName()) {
            index = i;
            break;
        }
    }
    return index;
}

// insertion sort
void Settings::sortCategories()
{
    QVector<Entry*> sorted;
    QString min = NULL;
    int minIndex;

    while(this->categories.count() > 1) {
        minIndex = 0;
        min = this->categories.at(minIndex)->getName();
        for(int i = 1; i < this->categories.count(); i++) {
            if(min.toUpper() > this->categories.at(i)->getName().toUpper()) {
                min = this->categories.at(i)->getName();
                minIndex = i;
            }
        }
        sorted.append(this->categories.at(minIndex));
        this->categories.removeAt(minIndex);
    }
    sorted.append(this->categories.at(0));
    this->categories = sorted;
}

void Settings::readSupportedDatatypes()
{
    QSqlQuery qry;
    QString error;
    if(!this->dbHandler->readSupportedDatatypes(&qry,&error)) {
        emit this->showWarning("Datentypen konnten nicht gelesen werden", error);
    }
    else {
        while(qry.next()) {
            this->supportedDatatypes.append(qry.value(0).toString());
        }
        emit this->showSupportedTypes(this->supportedDatatypes);
     }
}

void Settings::initCategories()
{
    QSqlQuery qry;
    QString error;

    if(!this->dbHandler->getCategories(&qry, &error)) {
        emit this->showWarning("Geraetetypen konnten nicht ausgelesen werden", error);
    }
    else {
        while(qry.next()) {
            this->categories.append(new Entry(qry.value(0).toString()));
        }
        emit this->showCategories(this->categories);
        this->initCustomfields();
    }
}

void Settings::initCustomfields()
{
    QSqlQuery qry;
    QString error;
    QString currentCategory = NULL;
    int categoryIndex = -1;

    for(int i = 0; i < this->categories.count(); i++) {
        currentCategory = this->categories.at(i)->getName();
        categoryIndex = this->getCategoryIndex(currentCategory);

        if(!this->dbHandler->getCustomfields(&qry, &error, currentCategory)) {
            emit this->showWarning("Auslesen der CustomFelder nicht möglich", error);
        }
        else {
            while(qry.next()) {
                Datafield* field = new Datafield(qry.value(0).toString(), qry.value(1).toInt());
                this->categories.at(categoryIndex)->addField(field);
            }
            emit this->showCustomfields(this->categories.at(categoryIndex)->getAllFields());
        }
    }
}

void Settings::createCategory(QString categoryName)
{
    QString error = "";
    bool categoryExists;

    if(!this->dbHandler->checkCategoryExists(categoryName, &categoryExists,&error)) {
        emit this->showWarning("Kategoriename konnte nicht auf Einzigartigkeit geprüft werden", error);
    }
    else if (categoryExists) {
        emit this->showInformation("Kategorie bereits vorhanden.");
    }
    else if(!this->dbHandler->createCategory(categoryName, &error)) {
        emit this->showWarning("Kategorie konnte nicht angelegt werden", error);
    }
    else {
        emit this->showInformation("Gerätetyp wurde angelegt.");
    }

    this->categories.append(new Entry(categoryName));
    this->sortCategories();
    emit this->showCategories(this->categories);
    emit setSettingsSelectedCategory(this->getCategoryIndex(categoryName));
}


void Settings::updateCategory(QString categoryName, QString newName)
{
    QString error = "";
    bool categoryExists = true;
    int categoryIndex = this->getCategoryIndex(categoryName);

    if(!this->dbHandler->checkCategoryExists(newName, &categoryExists, &error)) {
        emit this->showWarning("Kategorie konnte nicht gespeichert werden", error);
    }
    else if(categoryExists) {
        emit this->showInformation("Kategorie [" + newName + "] bereits vorhanden.");
    }
    else if(!this->dbHandler->updateCategory(categoryName, newName, &error)) {
        emit this->showWarning("Kategorie konnte nicht geändert werden", error);
    }
    else {
        this->categories.at(categoryIndex)->setName(newName);
        this->sortCategories();
        emit this->showCategories(this->categories);
        emit this->setSettingsSelectedCategory(this->getCategoryIndex(newName));
        emit this->showInformation("Kategorie wurde geändert.");
    }
}

void Settings::deleteCategory(QString category)
{
    //@TODO delete ALL entrys of given category
    QString error = NULL;
    int categoryIndex = this->getCategoryIndex(category);

    if(!this->dbHandler->deleteCategory(category, &error)) {
        emit this->showWarning("Kategorie konnte nicht gelöscht werden", error);
    }
    else {
        delete this->categories.at(categoryIndex);
        this->categories.removeAt(categoryIndex);
        emit this->showCategories(this->categories);
        emit this->showInformation("Kategorie wurde entfernt.");
    }
}

void Settings::createCustomfield()
{

}

void Settings::saveCustomfield()
{

}

void Settings::readCustomfield()
{

}

void Settings::deleteCustomfield(QString category, QString fieldname)
{

}
