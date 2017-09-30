#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString MainWindow::CREATE_OPERATOR = "Neu anlegen";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QObject::connect(this, SIGNAL(do_getCustomfields()), this , SLOT(getCustomFields()));
    this->ui->cb_category->addItem(MainWindow::CREATE_OPERATOR);
    this->ui->cb_customfield->addItem(MainWindow::CREATE_OPERATOR);
    this->getCategories();
    this->readSupportedDatatypes();
}

void MainWindow::readSupportedDatatypes()
{
    QSqlQuery qry;
    QString error;
    if(!this->dbHandler.readSupportedDatatypes(&qry,&error))
        QMessageBox::warning(this,"Fehler", "Datentypen konnten nicht gelsen werden: " + error);
    else
    {   this->ui->cb_customfieldType->clear();
        while(qry.next())
            this->ui->cb_customfieldType->addItem(qry.value(0).toString());
    }
}

void MainWindow::createCustomfield()
{
    QString error = NULL;
    bool customFieldExists = NULL;
    QString fieldName = this->ui->edt_customfieldName->text();
    QString categoryName = this->ui->cb_category->currentText();

    if(fieldName.isEmpty()) {
        QMessageBox::information(this, "Information", "Bitte Namen angeben.");
        return;
    }

    if(!this->dbHandler.checkCustomfieldExists(fieldName, categoryName, &customFieldExists, &error))
        QMessageBox::warning(this, "Fehler", "Custom-Feld konnte nicht erstellt werden: " + error);
    else if(customFieldExists)
        QMessageBox::information(this, "Information", "Custom-Feld bereits vorhanden.");
    else {
        QString error;
        QString name = this->ui->edt_customfieldName->text();
        QString datentyp = this->ui->cb_customfieldType->currentText();
        QString geraetetyp = this->ui->cb_category->currentText();
        bool pflichtfeld = this->ui->cb_customfieldRequired->isChecked();

        if(!this->dbHandler.createCustomField(&error,name,geraetetyp,datentyp,pflichtfeld))
            QMessageBox::warning(this, "Fehler", "Feld konnte nicht angelegt werden: " + error);
        else
        {
            emit do_getCustomfields();
            QMessageBox::information(this, "Information", "Datenfeld wurde angelegt");
        }
    }
}

void MainWindow::saveCustomfield()
{
    QString error;
    QString name = this->ui->edt_customfieldName->text();
    QString datentyp = this->ui->cb_customfieldType->currentText();
    QString geraetetyp = this->ui->cb_category->currentText();
    bool pflichtfeld = this->ui->cb_customfieldRequired->isChecked();

    if(!this->dbHandler.saveCustomField(name,geraetetyp, datentyp,pflichtfeld, &error))
        QMessageBox::warning(this, "Fehler", "Datenfeld konnte nicht geändert werden: " + error);
    else {
        if(!this->dbHandler.readCustomField(geraetetyp, &name, &datentyp, &pflichtfeld))
            QMessageBox::warning(this,"Fehler", "Datenfeld konnte nicht neu geladen werden: " + error);
        else {
            this->ui->edt_customfieldName->setText(name);
            this->ui->cb_customfieldType->setCurrentText(datentyp);
            this->ui->cb_customfieldRequired->setChecked(pflichtfeld);
        }
    }
}

void MainWindow::getCategories()
{
    QSqlQuery qry;
    QString error;

    this->categoriesReady = false;
    if(!this->dbHandler.getCategories(&qry, &error))
        QMessageBox::warning(this, "Fehler", "Geraetetypen konnten nicht ausgelesen werden: " + error);
    else
    {
        this->ui->cb_category->clear();
        this->ui->cb_category->addItem(MainWindow::CREATE_OPERATOR);
        while(qry.next())
            this->ui->cb_category->addItem(qry.value(0).toString());

        this->categoriesReady = true;
        emit this->do_getCustomfields();
    }
}

void MainWindow::getCustomFields() {
    QString current = this->ui->cb_category->currentText();
    if(current.isEmpty())
        return;

    QSqlQuery p_qry;
    QString error;
    if(!this->dbHandler.getCustomfields(&p_qry, &error, current))
        QMessageBox::warning(this, "Fehler", "Auslesen der CustomFelder nicht möglich: " + error);
    else
    {
        this->ui->cb_customfield->clear();
        this->ui->cb_customfield->addItem(MainWindow::CREATE_OPERATOR);
        while(p_qry.next())
            this->ui->cb_customfield->addItem(p_qry.value(0).toString());
    }
}

/********************************************************************************
 *                              UI-SLOTS                                        *
 ********************************************************************************/

void MainWindow::on_cb_category_currentIndexChanged(const QString &arg1)
{
    if(this->ui->cb_category->currentText() == MainWindow::CREATE_OPERATOR)
        this->ui->edt_categoryName->clear();
    else
        this->ui->edt_categoryName->setText(arg1);

    if(this->categoriesReady)
        emit this->do_getCustomfields();
}


void MainWindow::on_btn_categorySave_clicked()
{
    if(this->ui->edt_categoryName->text().length() <= 0)
    {
        QMessageBox::information(this,"Information", "Bitte Namen angeben.");
        return;
    }

    QString error = "";
    if(this->dbHandler.createCategory(this->ui->edt_categoryName->text(), &error))
        QMessageBox::information(this, "Information", "Geraetetyp wurde angelegt.");
    else
        QMessageBox::warning(this, "Fehler", error);

    this->ui->edt_categoryName->clear();
    this->getCategories();
}

void MainWindow::on_btn_customfieldSave_clicked()
{
    if(this->ui->cb_customfield->currentText() == MainWindow::CREATE_OPERATOR)
        this->createCustomfield();
    else
        this->saveCustomfield();
}

void MainWindow::on_cb_customfield_currentIndexChanged(const QString &arg1)
{
    if(arg1 == MainWindow::CREATE_OPERATOR) {
        this->ui->edt_customfieldName->clear();
    }
}
