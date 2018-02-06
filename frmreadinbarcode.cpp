#include "frmreadinbarcode.h"
#include "ui_frmreadinbarcode.h"

FrmReadInBarcode::FrmReadInBarcode(DBHandler *dbHandler, DialogController *dialogController, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmReadInBarcode)
{
    ui->setupUi(this);
    this->dbHandler = dbHandler;
    this->dialogController = dialogController;
    this->setFixedSize(this->width(),this->height());
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->init();
    this->hide();
}

FrmReadInBarcode::~FrmReadInBarcode()
{
    delete ui;
}

void FrmReadInBarcode::init()
{
    this->ui->cbManualInput->setChecked(false);
    this->manualInputMode = false;
    this->ui->edtBarcode->clear();
}

void FrmReadInBarcode::on_edtBarcode_returnPressed()
{
    QString barcode = this->ui->edtBarcode->text();
    if(!barcode.isEmpty() && this->isBarcodeAlreadyInDb(barcode)) {
        emit this->createObject(barcode);
    }
}

void FrmReadInBarcode::on_cbManualInput_toggled(bool checked)
{
    this->manualInputMode = checked;
}

void FrmReadInBarcode::on_edtBarcode_textChanged(const QString &barcode)
{
    if(!this->manualInputMode) {
        if(!barcode.isEmpty() && this->isBarcodeAlreadyInDb(barcode)) {
            emit this->createObject(barcode);
        }
    }
}

void FrmReadInBarcode::close()
{
    this->close();
}

void FrmReadInBarcode::showUp()
{
    this->init();
    this->show();
}

void FrmReadInBarcode::on_btnAbort_clicked()
{
    this->hide();
}

bool FrmReadInBarcode::isBarcodeAlreadyInDb(QString barcode)
{
    QString error = "";
    // available if code is already in db
    bool isAlreadyInDb = false;

    if(!this->dbHandler->checkObjectAvailability(barcode, &isAlreadyInDb, &error)) {
        emit this->dialogController->showWarning("Barcode konnte nicht geprüft werden", error);
    }
    else if(isAlreadyInDb) {
        emit this->dialogController->showInformation("Barcode bereits eingetragen.");
    }

    return isAlreadyInDb;
}
