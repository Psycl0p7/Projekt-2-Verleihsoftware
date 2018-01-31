#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVector>
#include "entry.h"
#include "dbhandler.h"
#include <QDebug>

class SettingsController : public QObject
{
    Q_OBJECT


public:
    static const QString CREATE_OPERATOR;
    SettingsController(DBHandler* dbHandler);
    ~SettingsController();

    void init();

    // categories
    int getCategoryIndex(QString name);
    void createCategory(QString categoryName);
    void updateCategory(QString categoryName, QString newName);
    void deleteCategory(QString category);

    // datafields
    int getDatafieldIndex(int categoryIndex, QString fieldname);
    void createCustomfield(QString fieldname, QString category, int typeIndex, bool isRequired);
    void updateCustomfield(QString category, QString currentFieldname, QString newFieldname, int newType, bool newRequired);
    void deleteCustomfield(QString category, QString fieldname);
    void switchCategoryDatafields(QString category);
    void switchDatafieldAttributes(QString category, QString fieldname);

private:
    QVector<Entry*> categories;
    QVector<QString> supportedDatatypes;
    DBHandler* dbHandler;


    void initCategories();
    void initCustomfields();
    void readSupportedDatatypes();


    void sortCategories();
    void sortDatafields(QString category);

signals:
    void showWarning(QString, QString);
    void showInformation(QString);

    void showSupportedTypes(QVector<QString>);
    void showCategories(QVector<Entry*>);
    void showDatafields(QVector<Datafield*>);
    void showDatafieldAttributes(QString name, int typeIndex, bool required);

    void setSettingsSelectedCategory(int);
    void setSettingsSelectedCustomfield(int);
};

#endif // SETTINGS_H
