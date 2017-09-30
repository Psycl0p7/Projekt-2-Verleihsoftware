#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dbhandler.h>
#include <QMessageBox>
#include <QVector>

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

private:
    Ui::MainWindow *ui;
    DBHandler dbHandler;
    bool categoriesReady;

    void init();
    void readSupportedDatatypes();

    void createCustomfield();
    void saveCustomfield();

private slots:
    void getCategories();
    void getCustomFields();

    void on_cb_category_currentIndexChanged(const QString &arg1);
    void on_btn_categorySave_clicked();
    void on_btn_customfieldSave_clicked();

    void on_cb_customfield_currentIndexChanged(const QString &arg1);

signals:
    void do_getCustomfields();

};

#endif // MAINWINDOW_H
