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

private slots:
    void on_btn_neuenTypAnlegen_clicked();
    void loadCustomFields();
    void gereateTypenEinlesen();

    void on_cb_typ_currentIndexChanged(const QString &arg1);

    void on_btn_saveCustomField_clicked();

private:
    Ui::MainWindow *ui;
    DBHandler dbHandler;

    bool geraeteTypenReady;

    void createNewCustomField();
    void saveCustomField();

    void init();
    void feldDatentypenEinlesen();
signals:
    void do_loadCustomFields();
public slots:
};

#endif // MAINWINDOW_H
