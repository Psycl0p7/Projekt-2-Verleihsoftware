#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dbhandler.h>
#include <QMessageBox>
#include <QVector>
#include <QDebug>
#include "settings.h"

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
    void showWarning(QString warning, QString error);
    void showInformation(QString information);

    void showSupportedTypes(QVector<QString> supportedTypes);
    void showCategories(QVector<Entry*> categories);
    void showDatafields(QVector<Datafield*> fields);

    void setSettingsSelectedCategory(int index);
    void setSettingsSelectedCustomfield(int index);
private:
    Ui::MainWindow *ui;
    DBHandler dbHandler;
    Settings *settings;
    bool categoriesReady;

    void init();

    void createCustomfield();
    void saveCustomfield();
    void readCustomfield();
    void deleteCustomfield(QString category, QString fieldname);

private slots:
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
