#ifndef FRMREADINBARCODE_H
#define FRMREADINBARCODE_H

#include <QWidget>

namespace Ui {
class FrmReadInBarcode;
}

class FrmReadInBarcode : public QWidget
{
    Q_OBJECT

public:
    explicit FrmReadInBarcode(QWidget *parent = 0);
    ~FrmReadInBarcode();

private slots:

    void on_edtBarcode_returnPressed();

    void on_cbManualInput_toggled(bool checked);

    void on_edtBarcode_textChanged(const QString &arg1);

    void on_btnAbort_clicked();

signals:
    void createObject(QString barcode);

private:
    Ui::FrmReadInBarcode *ui;
    bool manualInputMode = false;
};

#endif // FRMREADINBARCODE_H
