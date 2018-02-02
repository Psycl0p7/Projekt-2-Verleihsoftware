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
    GetCustomFieldsForTable();
    setDevicesInCombiBox();
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

/**
 * Holt sich die Kategorien und die CustomField aus der DB und zeigt diese in der List mit allen Geräten an.
 * @brief MainWindow::AddCustomValue
 */
void MainWindow::GetCustomFieldsForTable()
{
    QSqlQuery sql;
    QString error;
    int i = 0;

    dbHandler.getAllDeviceTypes(&sql, &error);
    while(sql.next()) {
        this->ui->tableWidget10->insertRow(0);
        this->ui->tableWidget10->insertRow(0);
        this->ui->tableWidget10->insertColumn(i);
        QTableWidgetItem *headerText = new QTableWidgetItem();

        headerText->setText(QString(sql.value(0).toString()));
        this->ui->tableWidget10->setHorizontalHeaderItem(i, headerText);
        i++;
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

/**
 * Holt sich alle Geräte aus der Datenbank und fügt diese in die ComboBox für eine Filterung ein.
 * @brief MainWindow::setDevicesInCombiBox
 */
void MainWindow::setDevicesInCombiBox()
{
    QSqlQuery sql;
    QString error;
    dbHandler.getCategories(&sql, &error);
    this->ui->tableWidget10->insertRow(0);
    this->ui->tableVerliehen->insertRow(0);
    while(sql.next()) {
        this->ui->deviceCat->addItem(sql.value(0).toString());
        this->ui->deviceVerliehen->addItem(sql.value(0).toString());
    }
}

/**
 * Holt sich alle Fields aus der Datenbank für die Ausgewählte Device-Category
 * @brief MainWindow::on_deviceCat_activated
 * @param arg1
 */
void MainWindow::on_deviceCat_activated(const QString &arg1)
{
    QSqlQuery sql;
    QString error;
    QString deviceName = this->ui->deviceCat->currentText();
    int i = 0;
    this->ui->tableWidget10->setColumnCount(0);
    if(this->ui->deviceCat->currentText() == SettingsController::CREATE_OPERATOR) {

    } else {
        dbHandler.getCustomfields(&sql, &error, deviceName);
        while(sql.next()) {
            this->ui->tableWidget10->insertColumn(0);
            QTableWidgetItem* header = new QTableWidgetItem;
            header->setText(QString(sql.value(0).toString()));
            this->ui->tableWidget10->setHorizontalHeaderItem(0, header);
            fillField(sql.value(0).toString(), deviceName, i);
            i++;
        }
    }
}

bool MainWindow::fillField(QString fieldName, QString category, int count)
{
    QSqlQuery sql;
    QString error;
    dbHandler.getAllDevicesForACategory(&sql, &error, fieldName, category);
    int i = 0;
    while(sql.next())
    {
        this->ui->tableWidget10->insertRow(0);
        this->ui->tableWidget10->setItem(i,count, new QTableWidgetItem(sql.value(0).toString()));
    }
    return true;
}

/**
 * Fügt eine neue Zeile in der Liste hinzu
 * @brief MainWindow::on_addBtn_clicked
 */
void MainWindow::on_addBtn_clicked() {
    this->ui->tableWidget10->insertRow(0);
}

/**
 * Speichert alle eingetragene Daten in der Datenbank
 * @brief MainWindow::on_saveBtn_clicked
 */
void MainWindow::on_saveBtn_clicked()
{
    QString catgeory = this->ui->deviceCat->currentText();
    QString ID;
    int len = this->ui->tableWidget10->rowCount();
    for(int i = 0; i < len; i++)
    {
        ID.clear();
        int len2 = this->ui->tableWidget10->columnCount();
        for (int j = 0; j < len2; j++)
        {
            QTableWidgetItem* item = this->ui->tableWidget10->item(i, j);
            QTableWidgetItem* header = this->ui->tableWidget10->horizontalHeaderItem(j);
            if(header->text().toUpper() == "BARCODE" || !ID.isEmpty())
            {
                if (checkIdIsCorrect(item)) {
                    ID = header->text();
                    CreateOrUpdateDatas(ID, item->text(), header->text(), catgeory);
                } else if (ID.isEmpty()){
                    QMessageBox::warning(this,"Fehler", "Es wurde keine gültiger Barcode eingetragen");
                }else {
                    CreateOrUpdateDatas(ID, item->text(), header->text(), catgeory);
                }
            }
        }
    }

}

/**
 * Checkt ob die eingegebene ID vorhanden ist
 * @brief MainWindow::checkIdIsCorrect
 */
bool MainWindow::checkIdIsCorrect(QTableWidgetItem* itemID)
{
    return itemID;
}

/**
 * Sucht anhand der ID/BarCode einen Daten und Updatet diesen oder erstellt einen neuen, wenn er nicht gefunden worden ist
 * @brief MainWindow::CreateOrUpdateDatas
 * @param id - Enthält die ID des gesuchten Geräts
 * @param data - Enthält den neuen oder alten Datensatz eines Felds
 * @param field - Enthält das Feld, in dem es gespeichert werden soll
 */
void MainWindow::CreateOrUpdateDatas(QString id, QString data, QString field, QString category)
{
    QSqlQuery sql;
    QString error = NULL;
    //dbHandler.findAndUpdateDevice(&sql, &error, id, data->text(), field->text());
    /*if(dbHandler.saveNewDeviceData(&sql, &error, id, data, field))
    {
        // QMessageBox::information(this, "Gespeichert", "Daten wurden erfolgreich gespeichert");
    } else {
        QMessageBox::warning(this, "Fehler", error + " Feld: " + field);
    }*/
dbHandler.existDeviceInDB(&sql, &error, data);
    if(sql.first()) {
        dbHandler.updateDevice(&sql, &error, id, data, field, category);
    } else {
        dbHandler.saveNewDeviceData(&sql, &error, id, data, field, category);
    }
}

/**
 * Beim Verleihen wird das Gerät in der DB suchen und die Felder aktualisiert
 * @brief MainWindow::on_verleihBtn_clicked
 */
void MainWindow::on_verleihBtn_clicked()
{
    this->ui->startTime->text();
    this->ui->endTime->text();
    this->ui->textVorname->text();
    this->ui->textVorname->text();
    this->ui->barcode->text();

    this->ui->barcode->clear();
    this->ui->barcode->focusWidget();
}

/**
 * Sucht anhand des eingegebenen Begriffs alle Geräte aus der DB, welche den Begriff als Attribute beinhalten
 * @brief MainWindow::on_searchBtn_clicked
 */
void MainWindow::on_searchBtn_clicked()
{
    QSqlQuery sql;
    QString error;
    QString searchMeta = this->ui->searchText->text();
    dbHandler.findAllSearchedData(&sql, &error, searchMeta);
}

/**
 * Holt sich alle Fields aus der Datenbank für die Ausgewählte Device-Category
 * @brief MainWindow::on_deviceCat_activated
 * @param arg1
 */
void MainWindow::on_deviceVerliehen_activated(const QString &arg1)
{
    QSqlQuery sql;
    QString error;
    QString deviceName = this->ui->deviceVerliehen->currentText();
    int i = 0;
    this->ui->tableVerliehen->setColumnCount(0);
    if(this->ui->deviceVerliehen->currentText() == SettingsController::CREATE_OPERATOR) {
    //TODO
    } else {
        dbHandler.getCustomfields(&sql, &error, deviceName);
        while(sql.next()) {
            this->ui->tableVerliehen->insertColumn(0);
            QTableWidgetItem* header = new QTableWidgetItem;
            header->setText(QString(sql.value(0).toString()));
            this->ui->tableVerliehen->setHorizontalHeaderItem(0, header);
            i++;
        }
    }
}
