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
    this->getCategoryNames();
}

void Settings::readSupportedDatatypes()
{
    QSqlQuery qry;
    QString error;
    if(!this->dbHandler->readSupportedDatatypes(&qry,&error)) {
        emit this->showWarning("Datentypen konnten nicht gelsen werden", error);
    }
    else {
        while(qry.next()) {
            this->supportedDatatypes.append(qry.value(0).toString());
        }
        emit this->showSupportedTypes(this->supportedDatatypes);
     }
}

void Settings::getCategoryNames()
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
    }
}

void Settings::getCustomFields()
{
    // QString current = this->ui->cb_category->currentText();
    QSqlQuery p_qry;
    QString error;

    // if(current.isEmpty() || current == MainWindow::CREATE_OPERATOR) {
    //     return;
    // }

    for(int i = 0; i < this->categories.count(); i++) {
        if(!this->dbHandler.getCustomfields(&p_qry, &error, this->categories.at(i)->getName())) {
            emit this->showWarning("Auslesen der CustomFelder nicht möglich", error);
        }
        else {
            // this->ui->cb_customfield->clear();
            // this->ui->cb_customfield->addItem(MainWindow::CREATE_OPERATOR);
            while(p_qry.next()) {
                // this->ui->cb_customfield->addItem(p_qry.value(0).toString());
                this->categories.at(i)->addField();
            }
        }
    }

}


void Settings::createCategory()
{

}

void Settings::saveCategory()
{

}

void Settings::deleteCategory(QString category)
{

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
