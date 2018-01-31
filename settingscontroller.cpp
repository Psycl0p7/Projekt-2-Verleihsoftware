#include "settingscontroller.h"

const QString SettingsController::CREATE_OPERATOR = "Neu anlegen";

SettingsController::SettingsController(DBHandler* dbHandler)
{
    this->dbHandler = dbHandler;
}

SettingsController::~SettingsController()
{

}

void SettingsController::init()
{
    this->readSupportedDatatypes();
    this->initCategories();
}

// sorting not required, qry is ordered by name asc
void SettingsController::initCategories()
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

// sorting not required, qry is ordered by name asc
void SettingsController::initCustomfields()
{
    QSqlQuery qry;
    QString error = "";
    QString currentCategory = "";
    int categoryIndex = -1;

    for(int i = 0; i < this->categories.count(); i++) {
        currentCategory = this->categories.at(i)->getName();
        categoryIndex = this->getCategoryIndex(currentCategory);

        if(!this->dbHandler->getCustomfields(&qry, &error, currentCategory)) {
            emit this->showWarning("Auslesen der dynamischen Datenfelder nicht möglich", error);
        }
        else {
            while(qry.next()) {
                Datafield* field = new Datafield(qry.value(0).toString(), qry.value(1).toInt());
                this->categories.at(categoryIndex)->addField(field);
            }
        }
    }
}

void SettingsController::readSupportedDatatypes()
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

int SettingsController::getCategoryIndex(QString name)
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

int SettingsController::getDatafieldIndex(int categoryIndex, QString fieldname)
{
    int index = -1;
    for(int i = 0; i < this->categories.at(categoryIndex)->countFields(); i++) {
        if(fieldname == this->categories.at(categoryIndex)->getField(i)->getName()) {
            index = i;
            break;
        }
    }
    return index;
}

// insertion sort
void SettingsController::sortCategories()
{
    QVector<Entry*> sorted;
    QString min = NULL;
    int minIndex = -1;

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

// insertion sort
void SettingsController::sortDatafields(QString category)
{
    int categoryIndex = this->getCategoryIndex(category);
    QVector<Datafield*> sorted;
    QString min = NULL;
    int minIndex = -1;

    if(categoryIndex == -1) {
        emit this->showWarning("Datenfelder konnten nicht sortiert werden",
                               "Kategorie [" + category + "] nicht gefunden.");
    }
    else {
        while(this->categories.at(categoryIndex)->countFields() > 1) {
            minIndex = 0;
            min = this->categories.at(categoryIndex)->getField(minIndex)->getName();
            for(int i = 1; i < this->categories.at(categoryIndex)->countFields(); i++) {
                if(min.toUpper() > this->categories.at(categoryIndex)->getField(i)->getName().toUpper()) {
                    min = this->categories.at(categoryIndex)->getField(i)->getName().toUpper();
                    minIndex = i;
                }
            }
            sorted.append(this->categories.at(categoryIndex)->getField(minIndex));
            this->categories.at(categoryIndex)->removeField(minIndex);
        }
        sorted.append(this->categories.at(categoryIndex)->getField(0));
        this->categories.at(categoryIndex)->setFields(sorted);
    }
}

void SettingsController::createCategory(QString categoryName)
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
    emit this->setSettingsSelectedCategory(this->getCategoryIndex(categoryName));
}

void SettingsController::updateCategory(QString categoryName, QString newName)
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

void SettingsController::deleteCategory(QString category)
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

// type id is always the combobox index
void SettingsController::createCustomfield(QString fieldname, QString category, int typeIndex, bool isRequired)
{
    QString error;
    bool customFieldExists = true;
    int categoryIndex = this->getCategoryIndex(category);

    if(!this->dbHandler->checkCustomfieldExists(fieldname, category, &customFieldExists, &error)) {
        emit this->showWarning("Datenfeld konnte nicht angelegt werden", error);
    }
    else if(customFieldExists) {
        emit this->showInformation("Datenfeld bereits vorhanden.");
    }
    else if(!this->dbHandler->createCustomField(&error, fieldname, category, typeIndex, isRequired)) {
        emit this->showWarning("Datenfeld konnte nicht angelegt werden", error);
    }
    else {
        this->categories.at(categoryIndex)->addField(new Datafield(fieldname, typeIndex, isRequired));
        this->sortDatafields(category);
        emit this->showDatafields(this->categories.at(categoryIndex)->getAllFields());
        emit this->showInformation("Datenfeld erfolgreich angelegt.");
    }
}

void SettingsController::switchCategoryDatafields(QString category)
{
    int categoryIndex = this->getCategoryIndex(category);
    emit this->showDatafields(this->categories.at(categoryIndex)->getAllFields());
}

void SettingsController::switchDatafieldAttributes(QString category, QString fieldname)
{
    int cIndex = this->getCategoryIndex(category);
    int fIndex = this->getDatafieldIndex(cIndex, fieldname);
    Datafield* field = this->categories.at(cIndex)->getField(fIndex);
    emit this->showDatafieldAttributes(field->getName(), field->getType(), field->isRequired());
}


void SettingsController::updateCustomfield(QString category, QString currentFieldname, QString newFieldname, int newType, bool newRequired)
{
    QString error = NULL;
    bool fieldExists = false;
    int categoryIndex = this->getCategoryIndex(category);
    int fieldIndex = this->getDatafieldIndex(categoryIndex, currentFieldname);

    if(!this->dbHandler->checkCustomfieldExists(currentFieldname, category, &fieldExists, &error)) {
        emit this->showWarning("Datenfeld konnte nicht gesucht werden", error);
    }
    else if (!fieldExists) {
        emit this->showWarning("Datenfeld konnte nicht gefunden werden", error);
    }
    else if(!this->dbHandler->updateCustomField(category, currentFieldname, newFieldname, newType, newRequired, &error)) {
        emit this->showWarning("Datenfeld konnte nicht geändert werden", error);
    }
    else {
        this->categories.at(categoryIndex)->getField(fieldIndex)->updateMeta(newFieldname, newType, newRequired);
        emit this->showDatafields(this->categories.at(categoryIndex)->getAllFields());
        emit this->setSettingsSelectedCustomfield(fieldIndex);
    }
}

void SettingsController::deleteCustomfield(QString category, QString fieldname)
{
    QString error = NULL;
    int categoryIndex = this->getCategoryIndex(category);
    int fieldIndex = this->getDatafieldIndex(categoryIndex, fieldname);
    if(!this->dbHandler->deleteCustomField(category, fieldname, &error)) {
        emit this->showWarning("Datenfeld konnte nicht gelöscht werden", error);
    }
    else {
        this->categories.at(categoryIndex)->removeField(fieldIndex);
        emit this->showDatafields(this->categories.at(categoryIndex)->getAllFields());
        emit this->showInformation("Datenfeld erfolgreich gelöscht.");
    }
}
