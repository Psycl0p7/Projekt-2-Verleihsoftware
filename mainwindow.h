#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dbhandler.h>
#include <QMessageBox>
#include <QVector>
#include <QDebug>
#include <QTableWidgetItem>
#include "settingscontroller.h"
#include "rentalcontroller.h"
#include "dialogcontroller.h"

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
    void showWarning(QString warning, QString error);
    void showInformation(QString information);

    void showSupportedTypes(QVector<QString> supportedTypes);
    void showCategories(QVector<Entry*> categories);
    void showDatafields(QVector<Datafield*> fields);
    void showDatafieldAttributes(QString name, int typeIndex, bool required);

    void showRentalEntries(QVector<Entry*> entries);
    void showRentalSelectedEntryData(QVector<Datafield*> fields);

    void setSettingsSelectedCategory(int index);
    void setSettingsSelectedCustomfield(int index);
private:
    Ui::MainWindow *ui;

    DBHandler dbHandler;
    DialogController dialogController;
    SettingsController *settingsController;
    RentalController* rentalController;

    bool categoriesReady;

    void init();
    void toggleCategoryActivated(bool activated);
    void initRentalEntryDetailTable();

private slots:
    void on_cb_category_currentIndexChanged(const QString &category);
    void on_btn_categorySave_clicked();
    void on_btn_customfieldSave_clicked();

    void on_cb_customfield_currentIndexChanged(const QString &fieldname);

    void on_btn_categoryDelete_clicked();

    void on_btn_customfieldDelete_clicked();
    void GetCustomFieldsForTable();
    void setDevicesInCombiBox();
    void on_deviceCat_activated(const QString &arg1);
    void on_addBtn_clicked();
    void on_saveBtn_clicked();
    bool checkIdIsCorrect(QTableWidgetItem* itemID);
    void CreateOrUpdateDatas(QString id, QString data, QString field, QString category);
    void on_searchBtn_clicked();
    void on_deviceVerliehen_activated(const QString &arg1);
    void fillField(QString cat);

    void on_cbRentEnterManually_toggled(bool checked);

    void on_btnRentEnterManually_clicked();

    void on_lwRentEntries_currentRowChanged(int currentRow);

signals:

};

#endif // MAINWINDOW_H
