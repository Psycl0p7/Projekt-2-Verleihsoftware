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
    void on_cb_typ_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    DBHandler dbHandler;

    void init();
    void gereateTypenEinlesen();
    void feldDatentypenEinlesen();
};

#endif // MAINWINDOW_H
