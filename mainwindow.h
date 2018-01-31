#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dbhandler.h>
#include <QMessageBox>
#include <QVector>
#include <QDebug>
#include <QTableWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    static const QString CREATE_OPERATOR;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:

private:
    Ui::MainWindow *ui;
    DBHandler dbHandler;
    bool categoriesReady;

    void init();
    void readSupportedDatatypes();

    void createCategory();
    void saveCategory();
    void deleteCategory(QString category);

    void createCustomfield();
    void saveCustomfield();
    void readCustomfield();
    void deleteCustomfield(QString category, QString fieldname);

private slots:
    void getCategories();
    void getCustomFields();

    void on_cb_category_currentIndexChanged(const QString &arg1);
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
    void CreateOrUpdateDatas(QString id, QString data, QString field);
    void on_verleihBtn_clicked();
    void on_searchBtn_clicked();
    void on_deviceVerliehen_activated(const QString &arg1);

signals:
    void do_getCustomfields();

};

#endif // MAINWINDOW_H
