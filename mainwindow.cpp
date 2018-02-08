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
    this->setFixedSize(this->width(),this->height());
    this->ui->btnObjectsCreate->setEnabled(false);
    this->ui->btnObjectsDelete->setEnabled(false);

    this->settingsController = new SettingsController(&this->dbHandler, &this->dialogController);
    this->objectController = new ObjectController(&this->dbHandler, &this->dialogController);
    this->rentalController = new RentalController(&this->dbHandler, &this->dialogController);


    this->categoriesReady = false;
    this->enterBarcodeManually = false;

    this->frmReadInBarcode = new FrmReadInBarcode(&this->dbHandler, &this->dialogController);

//   this->frmReadInBarcode->showUp();

    // ** SIGNAL SLOTS  **
    // dialog controller
    QObject::connect(&this->dialogController, SIGNAL(si_showInformation(QString)), this, SLOT(showInformation(QString)));
    QObject::connect(&this->dialogController, SIGNAL(si_showWarning(QString,QString)), this, SLOT(showWarning(QString,QString)));

    // settings controller
    QObject::connect(this->settingsController, SIGNAL(setSettingsSelectedCategory(int)), this, SLOT(setSettingsSelectedCategory(int)));
    QObject::connect(this->settingsController, SIGNAL(setSettingsSelectedCustomfield(int)), this, SLOT(setSettingsSelectedCustomfield(int)));
    QObject::connect(this->settingsController, SIGNAL(showSupportedTypes(QVector<QString>)), this, SLOT(showSupportedTypes(QVector<QString>)));
    QObject::connect(this->settingsController, SIGNAL(showCategories(QVector<Object*>)), this, SLOT(showCategories(QVector<Object*>)));
    QObject::connect(this->settingsController, SIGNAL(showDatafields(QVector<Datafield*>)), this, SLOT(showDatafields(QVector<Datafield*>)));
    QObject::connect(this->settingsController, SIGNAL(showDatafieldAttributes(QString,int,bool)), this, SLOT(showDatafieldAttributes(QString,int,bool)));
    // rental controller
    QObject::connect(this->rentalController, SIGNAL(showRentalEntries(QVector<Object*>)), this, SLOT(showRentalEntries(QVector<Object*>)));
    QObject::connect(this->rentalController, SIGNAL(showSelectedObjectData(QVector<Datafield*>)), this, SLOT(showRentalSelectedObjectData(QVector<Datafield*>)));
    QObject::connect(this->rentalController, SIGNAL(setSelectedObjectIndex(int)), this, SLOT(setSelectedObjectIndex(int)));
    QObject::connect(this->rentalController, SIGNAL(addRentalObject(QString)), this, SLOT(addRentalObject(QString)));
    QObject::connect(this->rentalController, SIGNAL(adjustObjectDataTableRows(int)), this, SLOT(adjustObjectDataTableRows(int)));
    QObject::connect(this->rentalController, SIGNAL(resetRentalView()), this, SLOT(resetRentalView()));

    // object controller
    QObject::connect(this->settingsController, SIGNAL(transmitCategories(QVector<Object*>)), this->objectController, SLOT(receiveCategories(QVector<Object*>)));
    QObject::connect(this->objectController, SIGNAL(resetTable(QVector<Datafield*>)), this, SLOT(resetObjectTable(QVector<Datafield*>)));
    QObject::connect(this->objectController, SIGNAL(showObjects(QVector<Object*>)), this, SLOT(showObjects(QVector<Object*>)));
    QObject::connect(this->frmReadInBarcode, SIGNAL(createObject(QString)), this->objectController, SLOT(createObject(QString)));
    QObject::connect(this->ui->twObjects, SIGNAL(itemChanged(QTableWidgetItem*)), this->objectController, SLOT(updateObject(QTableWidgetItem*)));
    this->settingsController->init();
    this->resetRentalView();
    this->initRentalObjectDetailTable();
    this->showActiveLents();
}

void MainWindow::initRentalObjectDetailTable()
{
    this->ui->twRentDetails->insertColumn(0);
    this->ui->twRentDetails->insertColumn(1);
    this->ui->twRentDetails->setHorizontalHeaderItem(0, new QTableWidgetItem("Feld"));
    this->ui->twRentDetails->setHorizontalHeaderItem(1, new QTableWidgetItem("Inhalt"));
    this->ui->twRentDetails->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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

void MainWindow::adjustObjectDataTableRows(int countFields)
{
    int countRows = this->ui->twRentDetails->rowCount();
    int neededOperations = 0;

    // adjust amount of rows
    if(countRows < countFields) {
        neededOperations = countFields - countRows;
        for(int i = 0; i < neededOperations; i++) {
            this->ui->twRentDetails->insertRow(0);
        }
    }
    else if(countRows > countFields) {
        neededOperations = countRows - countFields;
        for(int i = 0; i < neededOperations; i++) {
            this->ui->twRentDetails->removeRow(0);
        }
    }
}

void MainWindow::resetRentalView()
{
    QDateTime now = QDateTime::currentDateTime();

    this->ui->dtRentStart->setDateTime(now);
    this->ui->dtRentStart->setMinimumDateTime(now);
    this->ui->dtRentEnd->setDateTime(now);
    this->ui->dtRentEnd->setMinimumDateTime(now);
    // clear line edits
    this->ui->edtRentBarcode->clear();
    this->ui->edtRentExtra->clear();
    this->ui->edtRentFirstname->clear();
    this->ui->edtRentLastname->clear();

    // clear lists
    this->ui->lwRentEntries->clear();

    // init controller
    this->rentalController->init();
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

void MainWindow::showCategories(QVector<Object*> categories)
{
    this->categoriesReady = true;
    // in settings
    this->ui->cb_category->clear();
    this->ui->cb_category->addItem(SettingsController::CREATE_OPERATOR);
    for(int i = 0; i < categories.count(); i++) {
        this->ui->cb_category->addItem(categories.at(i)->getCategory());
    }

    // in objects
    this->ui->cbObjectsCategory->clear();
    for(int i = 0; i < categories.count(); i++) {
        this->ui->cbObjectsCategory->addItem(categories.at(i)->getCategory());
    }
    this->ui->cb_category->setCurrentIndex(0);
    if(categories.count() > 0) {
        this->ui->cbObjectsCategory->setCurrentIndex(0);
    }
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
    // first index is always the create operator
    this->ui->cb_customfield->setCurrentIndex(index + 1);
}

void MainWindow::showRentalEntries(QVector<Object *> objects)
{
    this->ui->lwRentEntries->clear();
    for(int i = 0; i < objects.count(); i++) {
        this->ui->lwRentEntries->addItem(objects.at(i)->getCategory());

    }
}
void MainWindow::setSelectedObjectIndex(int index)
{
    if(index < this->ui->lwRentEntries->count()) {
        this->ui->lwRentEntries->setCurrentRow(index);
    }
}

void MainWindow::addRentalObject(QString object)
{
    this->ui->lwRentEntries->addItem(object);
}

void MainWindow::resetObjectTable(QVector<Datafield *> datafields)
{
    QTableWidgetItem* headerItem;
    QFont requiredFont;
    QString headerText = "";

    this->objectController->setTableReady(false);

    requiredFont.setBold(true);
    this->ui->twObjects->clear();
    this->ui->twObjects->setColumnCount(datafields.count());
    for(int i = 0; i < datafields.count(); i++) {
        headerText = datafields.at(i)->getName();
        headerItem = new QTableWidgetItem(headerText);
        if(datafields.at(i)->isRequired()) {
            headerItem->setFont(requiredFont);
        }
        this->ui->twObjects->setHorizontalHeaderItem(i, headerItem);
    }
    this->ui->twObjects->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    this->objectController->searchObjectsByCategory(this->ui->cbObjectsCategory->currentIndex());
}

void MainWindow::showObjects(QVector<Object*> objects)
{
    this->objectController->setTableReady(false);
    this->ui->twObjects->clearContents();
    this->ui->twObjects->setRowCount(objects.count());

    for(int row = 0; row < objects.count(); row++) {
        for(int column = 0; column < objects.at(row)->countFields(); column++) {
            this->ui->twObjects->setItem(row, column, new QTableWidgetItem(objects.at(row)->getField(column)->getData()));
        }
    }
    this->objectController->setTableReady(true);
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
    if(fieldname == SettingsController::CREATE_OPERATOR) {
        this->ui->edt_customfieldName->clear();
        this->ui->cb_customfieldType->setCurrentIndex(0);
        this->ui->cb_customfieldRequired->setChecked(false);
    }
    else if(!this->ui->cb_customfield->currentText().isEmpty() && this->ui->cb_customfield->currentText() != SettingsController::CREATE_OPERATOR ) {
        this->settingsController->switchDatafieldAttributes(this->ui->cb_category->currentText(), this->ui->cb_customfield->currentText());
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

void MainWindow::showRentalSelectedObjectData(QVector<Datafield*> fields)
{
    //clear data, table adjustment already triggered by controller
    this->ui->twRentDetails->clearContents();
    // fill rows with data
    for(int i = 0; i < fields.count(); i++) {
        this->ui->twRentDetails->setItem(i, 0, new QTableWidgetItem(fields.at(i)->getName()));
        this->ui->twRentDetails->setItem(i, 1, new QTableWidgetItem(fields.at(i)->getData()));
    }
}

void MainWindow::on_cbRentEnterManually_toggled(bool checked)
{
    this->enterBarcodeManually = checked;
}

void MainWindow::on_lwRentEntries_currentRowChanged(int currentRow)
{
    this->rentalController->switchSelectedObject(currentRow);
}

void MainWindow::on_btnRentRemove_clicked()
{
    int selectedIndex = this->ui->lwRentEntries->currentRow();
    this->rentalController->removeSelectedObject(selectedIndex);
}

void MainWindow::on_edtRentBarcode_returnPressed()
{
    this->rentalController->tryAddObjectByBarcode(this->ui->edtRentBarcode->text());
    this->ui->edtRentBarcode->clear();
}

void MainWindow::on_edtRentBarcode_textChanged(const QString &changedText)
{
    if(!this->enterBarcodeManually && !changedText.isEmpty()) {
        this->rentalController->tryAddObjectByBarcode(changedText);
        this->ui->edtRentBarcode->clear();
    }
}

void MainWindow::on_btnRentNew_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Verleih verwerfen", "Wirklich zurücksetzen?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        this->resetRentalView();
    }
}

void MainWindow::on_btnRentalConfirm_clicked()
{
    QMessageBox::StandardButton reply;
    QString firstname = this->ui->edtRentFirstname->text();
    QString lastname = this->ui->edtRentLastname->text();
    QString extra = this->ui->edtRentExtra->toPlainText();
    QDateTime start = this->ui->dtRentStart->dateTime();
    QDateTime end = this->ui->dtRentEnd->dateTime();

    if(firstname.isEmpty() || lastname.isEmpty()) {
        this->showInformation("Bitte  Vor- und Nachname angeben.");
    }
    else {
        reply = QMessageBox::question(this, "Verleih bestätigen", "Sicher?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            this->rentalController->confirmActiveRental(firstname, lastname, extra, start, end);
        }
    }
}

void MainWindow::on_btnObjectsCreate_clicked()
{
    this->frmReadInBarcode->showUp();
}

void MainWindow::on_cbObjectsCategory_currentIndexChanged(int index)
{
    if(index > -1 && this->categoriesReady) {
        this->ui->btnObjectsCreate->setEnabled(true);
        this->ui->btnObjectsDelete->setEnabled(true);
        this->objectController->setSelectedCategory(index);
    }
    else {
        this->ui->btnObjectsCreate->setEnabled(false);
        this->ui->btnObjectsDelete->setEnabled(false);
    }
}

void MainWindow::on_btnObjectsUpdate_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Änderungen speichern", "Wollen Sie wirklich Ihre Änderungen speichern?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        this->objectController->updateToDatabase();
    }
}

void MainWindow::on_btnObjectsDiscard_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Änderungen verwerfen", "Wollen Sie wirklich Ihre Änderungen verwerfen?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        this->objectController->discardChanged();
    }
}

void MainWindow::on_btnObjectsDelete_clicked()
{
    int index = this->ui->twObjects->currentRow();
    if(index < 0) {
        this->showInformation("Kein Objekt ausgewählt.");
    }
    else {
        this->objectController->removeObject(index);
    }
}

void MainWindow::showActiveLents()
{
    QString verleihdauer;
    QVector<Rental*> list = this->rentalController->getAllLentDevice();
    this->ui->lwOverviewBorrower->clear();
    for(int i = 0; i < list.size(); i++)
    {
        qint64 dateEnd = list[i]->getEnd().toSecsSinceEpoch();
        qint64 dateNow = QDateTime::currentSecsSinceEpoch();
        qint64 dateDiff = dateEnd - dateNow;
        QListWidgetItem* pItem =new QListWidgetItem();
        if (dateDiff <= 0)
        {
            verleihdauer = "Verzug";
            pItem->setBackground(Qt::red); // sets green background
        } else {
            verleihdauer = "Ok";
        }
        pItem->setText(verleihdauer+ " - " + list[i]->getFirstname() + " " + list[i]->getLastname());
        this->ui->lwOverviewBorrower->addItem(pItem);
    }
}

void MainWindow::on_btnOverviewEndRental_clicked()
{
    if (this->ui->lwOverviewBorrower->currentRow() >= 0)
    {
        QSqlQuery sql;
        QString error;
        QVector<Rental*> list = this->rentalController->getAllLentDevice();
        QString idVar = list[this->ui->lwOverviewBorrower->currentRow()]->getID();
        dbHandler.closeLents(&sql, &error, idVar);
        clearAllFields();
        showActiveLents();
    } else {
        QMessageBox::warning(this, "Fehler", "Es wurde kein Verleihausgewählt");
    }

}

void MainWindow::clearAllFields()
{
this->ui->lwOverviewObjects->clear();
    this->ui->edtOverviewFirstname->clear();
    this->ui->edtOverviewLastname->clear();
    this->ui->dtOverviewStart->clear();
    this->ui->dtOverviewEnd->clear();
    this->ui->tbOverviewExtra->clear();
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (index == 0)
    {
        showActiveLents();
    }
}

void MainWindow::getCategory(QString id)
{
    QSqlQuery sql;
    QString error;
    dbHandler.getCategorieForLent(&sql, &error, id);
    while(sql.next())
    {
        dbHandler.getCategorieForLentNext(&sql, &error, sql.value(0).toString());
        while(sql.next())
        {
            this->ui->lwOverviewObjects->clear();
            this->ui->lwOverviewObjects->addItem(sql.value(0).toString());
        }

    }
}

void MainWindow::on_lwOverviewBorrower_currentRowChanged(int currentRow)
{
    QVector<Rental*> list = this->rentalController->getAllLentDevice();
    getCategory(list[currentRow]->getID());
    this->ui->edtOverviewFirstname->setText(list[currentRow]->getFirstname());
    this->ui->edtOverviewLastname->setText(list[currentRow]->getLastname());
    this->ui->dtOverviewStart->setDateTime(list[currentRow]->getStart());
    this->ui->dtOverviewEnd->setDateTime(list[currentRow]->getEnd());
    this->ui->tbOverviewExtra->setText(list[currentRow]->getExtra());
}
