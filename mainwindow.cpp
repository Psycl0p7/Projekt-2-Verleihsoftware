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
    this->settings = new Settings(&this->dbHandler);
    this->categoriesReady = false;

    QObject::connect(this->settings, SIGNAL(showInformation(QString)), this, SLOT(showInformation(QString)));
    QObject::connect(this->settings, SIGNAL(showWarning(QString,QString)), this, SLOT(showWarning(QString,QString)));
    QObject::connect(this->settings, SIGNAL(showSupportedTypes(QVector<QString>)), this, SLOT(showSupportedTypes(QVector<QString>)));
    QObject::connect(this->settings, SIGNAL(showCategories(QVector<Entry*>)), this, SLOT(showCategories(QVector<Entry*>)));

    this->settings->init();
}

// *** PUBLIC SLOTS **** //

void MainWindow::showWarning(QString warning, QString error)
{
    QMessageBox::warning(this, "Fehler", warning + ": " + error);
}

void MainWindow::showInformation(QString information)
{
    QMessageBox::information(this, "Information", information);
}

void MainWindow::showSupportedTypes(QVector<QString> supportedTypes)
{
    this->ui->cb_customfieldType->clear();
    for(int i = 0; i < supportedTypes.count();i++) {
        this->ui->cb_customfieldType->addItem(supportedTypes.at(i));
    }
}

void MainWindow::showCategories(QVector<Entry*> categories)
{   this->categoriesReady = false;
    this->ui->cb_category->clear();
    this->ui->cb_category->addItem(MainWindow::CREATE_OPERATOR);
    for(int i = 0; i < categories.count(); i++) {
        this->ui->cb_category->addItem(categories.at(i)->getName());
    }
    this->categoriesReady = true;
}

// ******************************************


void MainWindow::createCategory()
{
    QString error = "";
    bool categoryExists;
    QString newCategoryName = this->ui->edt_categoryName->text();

    if(!this->dbHandler.checkCategoryExists(newCategoryName, &categoryExists,&error)) {
        QMessageBox::warning(this,"Fehler", "Kategorie konnte nicht angelget werden: " + error);
    }
    else if (categoryExists) {
        QMessageBox::information(this,"Information", "Kategorie bereits vorhanden.");
    }
    else if(!this->dbHandler.createCategory(newCategoryName, &error)) {
        QMessageBox::warning(this, "Fehler", error);
    }
    else {
        QMessageBox::information(this, "Information", "Gerätetyp wurde angelegt.");
    }

    //this->getCategories();
    this->ui->cb_category->setCurrentText(newCategoryName);
}

void MainWindow::saveCategory()
{
    QString error = "";
    bool categoryExists;
    QString newName = this->ui->edt_categoryName->text();

    if(!this->dbHandler.checkCategoryExists(this->ui->edt_categoryName->text(), &categoryExists, &error)) {
        QMessageBox::warning(this, "Fehler", "Kategories konnte nicht gespeichert werden: " + error);
    }
    else if(categoryExists) {
        QMessageBox::information(this, "Information", "Kategorie bereits vorhanden.");
    }
    else if(!this->dbHandler.updateCategory(this->ui->cb_category->currentText(), newName, &error)) {
        QMessageBox::warning(this, "Fehler", "Kategorie konnte nicht gespeichert werden: " + error);
    }
    else {
        QMessageBox::information(this, "Information", "Kategorie wurde geändert.");
    }

    //this->getCategories();
    this->ui->cb_category->setCurrentText(newName);
}

void MainWindow::deleteCategory(QString category)
{
    QString error = NULL;

    if(!this->dbHandler.deleteCategory(category, &error)) {
        QMessageBox::warning(this, "Fehler", "Kategorie konnte nicht gelöscht werden: " + error);
    }
    else {
        QMessageBox::information(this, "Information", "Kategorie wurde entfernt.");
    }
    //this->getCategories();
}

void MainWindow::createCustomfield()
{
    QString error = NULL;
    bool customFieldExists = NULL;
    QString fieldName = this->ui->edt_customfieldName->text();
    QString categoryName = this->ui->cb_category->currentText();

    if(fieldName.isEmpty()) {
        QMessageBox::information(this, "Information", "Bitte Bezeichnung angeben.");
        return;
    }

    if(!this->dbHandler.checkCustomfieldExists(fieldName, categoryName, &customFieldExists, &error)) {
        QMessageBox::warning(this, "Fehler", "Custom-Feld konnte nicht erstellt werden: " + error);
    }
    else if(customFieldExists) {
        QMessageBox::information(this, "Information", "Custom-Feld bereits vorhanden.");
    }
    else {
        QString error;
        QString name = this->ui->edt_customfieldName->text();
        QString datentyp = this->ui->cb_customfieldType->currentText();
        QString geraetetyp = this->ui->cb_category->currentText();
        bool required = this->ui->cb_customfieldRequired->isChecked();

        if(!this->dbHandler.createCustomField(&error,name,geraetetyp,datentyp,required)) {
            QMessageBox::warning(this, "Fehler", "Feld konnte nicht angelegt werden: " + error);
        }
        else {
            emit do_getCustomfields();
            QMessageBox::information(this, "Information", "Datenfeld wurde angelegt");
            this->ui->cb_category->setCurrentText(categoryName);
            this->ui->cb_customfield->setCurrentText(fieldName);
        }
    }
}

void MainWindow::saveCustomfield()
{
    QString error = NULL;
    bool fieldExists = NULL;
    QString categoryName = this->ui->cb_category->currentText();
    QString currentFieldname = this->ui->cb_customfield->currentText();
    QString newFieldname = this->ui->edt_customfieldName->text();
    QString datentyp = this->ui->cb_customfieldType->currentText();
    bool required = this->ui->cb_customfieldRequired->isChecked();

    int currentCustomfieldIndex = this->ui->cb_customfield->currentIndex();

    if(!this->dbHandler.checkCustomfieldExists(currentFieldname, categoryName, &fieldExists, &error)) {
        QMessageBox::warning(this, "Fehler", error);
    }
    else if (!fieldExists) {
        QMessageBox::information(this, "Information", "Feld konnte nicht gespeichert werden, da es nicht gefunden werden konnte.");
    }
    else if(!this->dbHandler.updateCustomField(categoryName, currentFieldname, newFieldname, datentyp, required, &error)) {
        QMessageBox::warning(this, "Fehler", "Datenfeld konnte nicht geändert werden: " + error);
    }
    else {
        emit this->do_getCustomfields();
        this->ui->cb_customfield->setCurrentIndex(currentCustomfieldIndex);
    }
}

void MainWindow::readCustomfield()
{   QString error = NULL;
    QString fieldname = this->ui->cb_customfield->currentText();
    QString category = this->ui->cb_category->currentText();
    QString datatype = NULL;
    bool required = NULL;

    if(!this->dbHandler.readCustomField(category, fieldname, &fieldname, &datatype, &required)) {
        QMessageBox::warning(this, "Fehler", "Auslesen der Feldinfrmationen nicht möglich: " + error);
    } else {
        this->ui->cb_customfield->setCurrentText(fieldname);
        this->ui->edt_customfieldName->setText(fieldname);
        this->ui->cb_customfieldRequired->setChecked(required);
        this->ui->cb_customfieldType->setCurrentText(datatype);
    }
}

void MainWindow::deleteCustomfield(QString category, QString fieldname)
{
    QString error = NULL;
    if(!this->dbHandler.deleteCustomField(category, fieldname, &error)) {
        QMessageBox::warning(this, "Fehler", "Datenfeld konnte nicht gelöscht werden: " + error);
    }
    else {
        QMessageBox::information(this, "Information", "Datenfeld erfolgreich gelöscht.");
    }
}


/********************************************************************************
 *                              UI-SLOTS                                        *
 ********************************************************************************/

void MainWindow::on_cb_category_currentIndexChanged(const QString &arg1)
{
    if(this->ui->cb_category->currentText() == MainWindow::CREATE_OPERATOR) {
        this->ui->edt_categoryName->clear();
    }
    else {
        this->ui->edt_categoryName->setText(arg1);
    }

    if(this->categoriesReady) {
        emit this->do_getCustomfields();
    }
}


void MainWindow::on_btn_categorySave_clicked()
{
    if(this->ui->edt_categoryName->text().length() <= 0) {
        QMessageBox::information(this,"Information", "Bitte Bezeichnung angeben.");
    }
    else if(this->ui->cb_category->currentText() == MainWindow::CREATE_OPERATOR) {
        this->createCategory();
    }
    else if(this->ui->cb_category->currentText() != this->ui->edt_categoryName->text()) {
        this->saveCategory();
    }

}

void MainWindow::on_btn_customfieldSave_clicked()
{
    if(this->ui->cb_customfield->currentText() == MainWindow::CREATE_OPERATOR) {
        this->createCustomfield();
    }
    else {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Datenfeld ändern", "Sind Sie sicher?",
                                      QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes) {
            this->saveCustomfield();
        }
    }

}

void MainWindow::on_cb_customfield_currentIndexChanged(const QString &fieldname)
{
    if(fieldname == MainWindow::CREATE_OPERATOR) {
        this->ui->edt_customfieldName->clear();
        this->ui->cb_customfieldType->setCurrentIndex(0);
        this->ui->cb_customfieldRequired->setChecked(false);
    }
    else if(!this->ui->cb_customfield->currentText().isEmpty() && this->ui->cb_customfield->currentText() != MainWindow::CREATE_OPERATOR ) {
        this->readCustomfield();
    }
}

void MainWindow::on_btn_categoryDelete_clicked()
{
    if(this->ui->cb_category->currentText() == MainWindow::CREATE_OPERATOR) {
        QMessageBox::information(this, "Information", "Keine Kategorie ausgewählt.");
        return;

    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Kategorie löschen", "Somit wird die Kategroie und damit alle verbundenen "
                                                             "Daten unwiederruflich gelöscht. Sind Sie sicher?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        this->deleteCategory(this->ui->cb_category->currentText());
    }
}

void MainWindow::on_btn_customfieldDelete_clicked()
{
    QString category = this->ui->cb_category->currentText();
    QString fieldname = this->ui->cb_customfield->currentText();

    if(category == MainWindow::CREATE_OPERATOR || fieldname == MainWindow::CREATE_OPERATOR) {
        QMessageBox::information(this, "Information", "Kein Feld ausgewählt.");
    }
    else if(QMessageBox::question(this, "Datenfeld löschen", "Datenfeld wirklich löschen?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        this->deleteCustomfield(category, fieldname);
        emit this->do_getCustomfields();
        this->ui->cb_customfield->setCurrentIndex(0);
    }
}
