#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dbhandler.h>
#include <QMessageBox>
#include <QVector>
#include <QDebug>
#include "settingscontroller.h"

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

    void setSettingsSelectedCategory(int index);
    void setSettingsSelectedCustomfield(int index);
private:
    Ui::MainWindow *ui;

    DBHandler dbHandler;
    SettingsController *settingsController;

    bool categoriesReady;

    void init();
    void toggleCategoryActivated(bool activated);

private slots:
    void on_cb_category_currentIndexChanged(const QString &category);
    void on_btn_categorySave_clicked();
    void on_btn_customfieldSave_clicked();

    void on_cb_customfield_currentIndexChanged(const QString &fieldname);

    void on_btn_categoryDelete_clicked();

    void on_btn_customfieldDelete_clicked();

signals:

};

#endif // MAINWINDOW_H
