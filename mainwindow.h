#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dbhandler.h>
#include <QMessageBox>
#include <QVector>
#include <QDebug>

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

signals:
    void do_getCustomfields();

};

#endif // MAINWINDOW_H
