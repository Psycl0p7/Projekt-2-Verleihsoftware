#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dbhandler.h>
#include <QMessageBox>
#include <QVector>
#include <QDebug>
#include <QTableWidgetItem>
#include <QCloseEvent>
#include "settingscontroller.h"
#include "rentalcontroller.h"
#include "dialogcontroller.h"
#include "objectcontroller.h"
#include "frmreadinbarcode.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:

    // *** UI - Controlling *** //

    // dialogs
    void showWarning(QString warning, QString error);
    void showInformation(QString information);

    // settings view
    void showSupportedTypes(QVector<QString> supportedTypes);
    void showCategories(QVector<Object*> categories);
    void showDatafields(QVector<Datafield*> fields);
    void showDatafieldAttributes(QString name, int typeIndex, bool required);
    void setSettingsSelectedCategory(int index);
    void setSettingsSelectedCustomfield(int index);

    // rental view
    void setSelectedObjectIndex(int index);
    void addRentalObject(QString objectCategory);
    void showRentalEntries(QVector<Object*> objects);
    void showRentalSelectedObjectData(QVector<Datafield*> fields);
    void adjustObjectDataTableRows(int countFields);
    void resetRentalView();

    // object view
    void addObjectToTable(Object* object);
    void showObjects(QVector<Object*> objects);
    void resetObjectTable(QVector<Datafield*> datafields);

    // ***                  *** //

private:
    Ui::MainWindow *ui;

    DBHandler dbHandler;
    DialogController dialogController;
    SettingsController *settingsController;
    RentalController* rentalController;
    ObjectController* objectController;

    FrmReadInBarcode* frmReadInBarcode;

    bool categoriesReady;
    bool enterBarcodeManually;

    void init();
    void toggleCategoryActivated(bool activated);
    void initRentalObjectDetailTable();

private slots:
    void on_cb_category_currentIndexChanged(const QString &category);

    void on_btn_categorySave_clicked();

    void on_btn_customfieldSave_clicked();

    void on_cb_customfield_currentIndexChanged(const QString &fieldname);

    void on_btn_categoryDelete_clicked();

    void on_btn_customfieldDelete_clicked();

    void on_cbRentEnterManually_toggled(bool checked);

    void on_lwRentEntries_currentRowChanged(int currentRow);

    void on_btnRentRemove_clicked();

    void on_edtRentBarcode_returnPressed();

    void on_edtRentBarcode_textChanged(const QString &changedText);

    void on_btnRentNew_clicked();

    void on_btnRentalConfirm_clicked();

    void on_btnObjectsCreate_clicked();

    void on_cbObjectsCategory_currentIndexChanged(int index);

signals:

};

#endif // MAINWINDOW_H
