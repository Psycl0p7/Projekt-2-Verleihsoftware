#include "frmreadinbarcode.h"
#include "ui_frmreadinbarcode.h"

FrmReadInBarcode::FrmReadInBarcode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmReadInBarcode)
{
    ui->setupUi(this);
    this->ui->cbManualInput->setChecked(false);
}

FrmReadInBarcode::~FrmReadInBarcode()
{
    delete ui;
}

void FrmReadInBarcode::on_edtBarcode_returnPressed()
{
    // do smtmth
}

void FrmReadInBarcode::on_cbManualInput_toggled(bool checked)
{
    this->manualInputMode = checked;
}

void FrmReadInBarcode::on_edtBarcode_textChanged(const QString &arg1)
{
    if(!this->manualInputMode) {
        // do smth
    }
}

void FrmReadInBarcode::on_btnAbort_clicked()
{

}
