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
    GetCustomFieldsForTable();


    QObject::connect(this, SIGNAL(do_getCustomfields()), this , SLOT(getCustomFields()));
    this->ui->cb_category->addItem(MainWindow::CREATE_OPERATOR);
    this->ui->cb_customfield->addItem(MainWindow::CREATE_OPERATOR);
    this->getCategories();
    this->readSupportedDatatypes();
    setDevicesInCombiBox();
}


void MainWindow::readSupportedDatatypes()
{
    QSqlQuery qry;
    QString error;
    if(!this->dbHandler.readSupportedDatatypes(&qry,&error)) {
        QMessageBox::warning(this,"Fehler", "Datentypen konnten nicht gelsen werden: " + error);
    }
    else {
        this->ui->cb_customfieldType->clear();
        while(qry.next()) {
            this->ui->cb_customfieldType->addItem(qry.value(0).toString());
        }
    }
}

void MainWindow::getCategories()
{
    QSqlQuery qry;
    QString error;

    this->categoriesReady = false;
    if(!this->dbHandler.getCategories(&qry, &error)) {
        QMessageBox::warning(this, "Fehler", "Geraetetypen konnten nicht ausgelesen werden: " + error);
    }
    else {
        this->ui->cb_category->clear();
        this->ui->cb_category->addItem(MainWindow::CREATE_OPERATOR);
        while(qry.next()) {
            this->ui->cb_category->addItem(qry.value(0).toString());
        }

        this->categoriesReady = true;
        emit this->do_getCustomfields();
    }
}

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

    this->getCategories();
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

    this->getCategories();
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
    this->getCategories();
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

void MainWindow::getCustomFields()
{
    QString current = this->ui->cb_category->currentText();
    QSqlQuery p_qry;
    QString error;

    if(current.isEmpty() || current == MainWindow::CREATE_OPERATOR) {
        return;
    }

    if(!this->dbHandler.getCustomfields(&p_qry, &error, current)) {
        QMessageBox::warning(this, "Fehler", "Auslesen der CustomFelder nicht möglich: " + error);
    }
    else {
        this->ui->cb_customfield->clear();
        this->ui->cb_customfield->addItem(MainWindow::CREATE_OPERATOR);
        while(p_qry.next())
            this->ui->cb_customfield->addItem(p_qry.value(0).toString());
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

    if(category == MainWindow::CREATE_OPERATOR || fieldname == MainWindow::CREATE_OPERATOR) {
        QMessageBox::information(this, "Information", "Kein Feld ausgewählt.");
    }
    else if(QMessageBox::question(this, "Datenfeld löschen", "Datenfeld wirklich löschen?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        this->deleteCustomfield(category, fieldname);
        emit this->do_getCustomfields();
        this->ui->cb_customfield->setCurrentIndex(0);
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
    if(this->ui->deviceCat->currentText() == MainWindow::CREATE_OPERATOR) {

    } else {
        dbHandler.getCustomfields(&sql, &error, deviceName);
        while(sql.next()) {
            this->ui->tableWidget10->insertColumn(0);
            QTableWidgetItem* header = new QTableWidgetItem;
            header->setText(QString(sql.value(0).toString()));
            this->ui->tableWidget10->setHorizontalHeaderItem(0, header);
            i++;
        }
    }
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
                    CreateOrUpdateDatas(ID, item->text(), header->text());
                } else if (ID.isEmpty()){
                    QMessageBox::warning(this,"Fehler", "Es wurde keine gültiger Barcode eingetragen");
                }else {
                    CreateOrUpdateDatas(ID, item->text(), header->text());
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
 * @param data - ENthält den neuen oder alten Datensatz eines Felds
 * @param field - Enthält das Feld, in dem es gespeichert werden soll
 */
void MainWindow::CreateOrUpdateDatas(QString id, QString data, QString field)
{
    QSqlQuery sql;
    QString error;
    //dbHandler.findAndUpdateDevice(&sql, &error, id, data->text(), field->text());
    if(dbHandler.saveNewDeviceData(&sql, &error, id, data, field))
    {
        // QMessageBox::information(this, "Gespeichert", "Daten wurden erfolgreich gespeichert");
    } else {
        QMessageBox::warning(this, "Fehler", error + " Feld: " + field);
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
    if(this->ui->deviceVerliehen->currentText() == MainWindow::CREATE_OPERATOR) {
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
