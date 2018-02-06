#ifndef FRMREADINBARCODE_H
#define FRMREADINBARCODE_H

#include <QWidget>
#include <dbhandler.h>
#include "dialogcontroller.h"

namespace Ui {
class FrmReadInBarcode;
}

class FrmReadInBarcode : public QWidget
{
    Q_OBJECT

public:
    explicit FrmReadInBarcode(DBHandler *dbHandler, DialogController *dialogController ,QWidget *parent = 0);
    ~FrmReadInBarcode();

    void showUp();
    void close();

private slots:

    void on_edtBarcode_returnPressed();

    void on_cbManualInput_toggled(bool checked);

    void on_edtBarcode_textChanged(const QString &barcode);

    void on_btnAbort_clicked();

signals:
    void createObject(QString barcode);

private:
    Ui::FrmReadInBarcode *ui;
    DBHandler* dbHandler;
    DialogController* dialogController;

    bool manualInputMode = false;
    bool isBarcodeAvailable(QString barcode);
    void init();
};

#endif // FRMREADINBARCODE_H
