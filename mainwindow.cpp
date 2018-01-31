#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    this->settingsController = new SettingsController(&this->dbHandler);
    this->categoriesReady = false;


    // ** SIGNAL SLOTS  **
    QObject::connect(this->settingsController, SIGNAL(setSettingsSelectedCategory(int)), this, SLOT(setSettingsSelectedCategory(int)));
    QObject::connect(this->settingsController, SIGNAL(setSettingsSelectedCustomfield(int)), this, SLOT(setSettingsSelectedCustomfield(int)));

    QObject::connect(this->settingsController, SIGNAL(showInformation(QString)), this, SLOT(showInformation(QString)));
    QObject::connect(this->settingsController, SIGNAL(showWarning(QString,QString)), this, SLOT(showWarning(QString,QString)));
    QObject::connect(this->settingsController, SIGNAL(showSupportedTypes(QVector<QString>)), this, SLOT(showSupportedTypes(QVector<QString>)));
    QObject::connect(this->settingsController, SIGNAL(showCategories(QVector<Entry*>)), this, SLOT(showCategories(QVector<Entry*>)));
    QObject::connect(this->settingsController, SIGNAL(showDatafields(QVector<Datafield*>)), this, SLOT(showDatafields(QVector<Datafield*>)));
    QObject::connect(this->settingsController, SIGNAL(showDatafieldAttributes(QString,int,bool)), this, SLOT(showDatafieldAttributes(QString,int,bool)));

    this->settingsController->init();
}

void MainWindow::toggleCategoryActivated(bool activated)
{
    if(activated) {
        // enable on category functionalities
        this->ui->btn_categoryDelete->setEnabled(activated);
        this->ui->gb_settingsCustomfields->setEnabled(activated);
    }
    else {
        // clear inputs
        this->ui->edt_categoryName->clear();
        this->ui->edt_customfieldName->clear();
        this->ui->cb_customfieldRequired->setChecked(false);
        this->ui->cb_customfield->setCurrentIndex(0);
        this->ui->cb_customfieldType->setCurrentIndex(0);

        // disable on category functionalities
        this->ui->btn_categoryDelete->setEnabled(activated);
        this->ui->gb_settingsCustomfields->setEnabled(activated);
    }
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
{
    this->ui->cb_category->clear();
    this->ui->cb_category->addItem(SettingsController::CREATE_OPERATOR);
    for(int i = 0; i < categories.count(); i++) {
        this->ui->cb_category->addItem(categories.at(i)->getName());
    }
    this->ui->cb_category->setCurrentIndex(0);
}

void MainWindow::showDatafields(QVector<Datafield*> fields)
{
    this->ui->cb_customfield->clear();
    this->ui->cb_customfield->addItem(SettingsController::CREATE_OPERATOR);
    for(int i = 0; i < fields.count(); i++) {
        this->ui->cb_customfield->addItem(fields.at(i)->getName());
    }
    this->ui->cb_customfield->setCurrentIndex(0);
}

void MainWindow::showDatafieldAttributes(QString name, int typeIndex, bool required)
{
    this->ui->edt_customfieldName->setText(name);
    this->ui->cb_customfieldType->setCurrentIndex(typeIndex);
    this->ui->cb_customfieldRequired->setChecked(required);
}

void MainWindow::setSettingsSelectedCategory(int index)
{
    // first index is always the create operator
    this->ui->cb_category->setCurrentIndex(index + 1);
}

void MainWindow::setSettingsSelectedCustomfield(int index)
{
    this->ui->cb_customfield->setCurrentIndex(index);
}

/********************************************************************************
 *                              UI-SLOTS                                        *
 ********************************************************************************/

void MainWindow::on_cb_category_currentIndexChanged(const QString &category)
{
    if(category.isEmpty()) {
        return;
    }
    if(category == SettingsController::CREATE_OPERATOR) {
        this->toggleCategoryActivated(false);
    }
    else {
        this->ui->edt_categoryName->setText(category);
        this->settingsController->switchCategoryDatafields(category);
        this->toggleCategoryActivated(true);
    }
}

void MainWindow::on_btn_categorySave_clicked()
{
    QString category_cb = this->ui->cb_category->currentText();
    QString category_edt = this->ui->edt_categoryName->text();
    if(category_edt.length() < 1) {
        QMessageBox::information(this,"Information", "Bitte Bezeichnung angeben.");
    }
    else if(category_cb == SettingsController::CREATE_OPERATOR) {
        this->settingsController->createCategory(category_edt);
    }
    else if(category_cb != category_edt) {
        this->settingsController->updateCategory(category_cb, category_edt);
    }
}


void MainWindow::on_btn_customfieldSave_clicked()
{
    QString category = this->ui->cb_category->currentText();
    QString fieldname_cb = this->ui->cb_customfield->currentText();
    QString fieldname_edt = this->ui->edt_customfieldName->text();
    bool isRequired = this->ui->cb_customfieldRequired->isChecked();
    int type = this->ui->cb_customfieldType->currentIndex();

    if(fieldname_edt.isEmpty()) {
        emit this->showInformation("Bitte Bezeichnung angeben.");
    }
    else if(fieldname_cb == SettingsController::CREATE_OPERATOR) {
        this->settingsController->createCustomfield(fieldname_edt, category, type, isRequired);
    }
    else {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Datenfeld ändern", "Sind Sie sicher?",
                                      QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes) {
            this->settingsController->updateCustomfield(category, fieldname_cb, fieldname_edt, type, isRequired);
        }
    }
}

void MainWindow::on_cb_customfield_currentIndexChanged(const QString &fieldname)
{
    if(fieldname.isEmpty()) {
        return;
    }
    QString category = this->ui->cb_category->currentText();
    if(fieldname == SettingsController::CREATE_OPERATOR) {
        this->ui->edt_customfieldName->clear();
        this->ui->cb_customfieldType->setCurrentIndex(0);
        this->ui->cb_customfieldRequired->setChecked(false);
    }
    else if(!this->ui->cb_customfield->currentText().isEmpty() && this->ui->cb_customfield->currentText() != SettingsController::CREATE_OPERATOR ) {
        this->ui->edt_customfieldName->setText(fieldname);
    }
}

void MainWindow::on_btn_categoryDelete_clicked()
{
    QString selectedCategory = this->ui->cb_category->currentText();
    if(selectedCategory == SettingsController::CREATE_OPERATOR) {
        this->showInformation("Keine Kategorie ausgewählt.");
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Kategorie löschen", "Somit wird die Kategroie und damit alle verbundenen "
                                                             "Daten unwiederruflich gelöscht. Sind Sie sicher?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        this->settingsController->deleteCategory(selectedCategory);
    }
}

void MainWindow::on_btn_customfieldDelete_clicked()
{
    QString category = this->ui->cb_category->currentText();
    QString fieldname = this->ui->cb_customfield->currentText();

    if(category == SettingsController::CREATE_OPERATOR || fieldname == SettingsController::CREATE_OPERATOR) {
        QMessageBox::information(this, "Information", "Kein Feld ausgewählt.");
    }
    else if(QMessageBox::question(this, "Datenfeld löschen", "Datenfeld wirklich löschen?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        this->settingsController->deleteCustomfield(category, fieldname);
    }
}
