#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVector>
#include "entry.h"
#include "dbhandler.h"
#include <QDebug>

class Settings : public QObject
{
    Q_OBJECT

public:
    Settings(DBHandler* dbHandler);
    ~Settings();

    void init();

    int getCategoryIndex(QString name);
    void createCategory(QString categoryName);
    void updateCategory(QString categoryName, QString newName);
    void deleteCategory(QString category);

private:
    QVector<Entry*> categories;
    QVector<QString> supportedDatatypes;
    DBHandler* dbHandler;

    void readSupportedDatatypes();

    void initCategories();
    void initCustomfields();

    void createCustomfield();
    void saveCustomfield();
    void readCustomfield();
    void deleteCustomfield(QString category, QString fieldname);

    void sortCategories();

signals:
    void showWarning(QString, QString);
    void showInformation(QString);

    void showSupportedTypes(QVector<QString>);
    void showCategories(QVector<Entry*>);
    void showCustomfields(QVector<Datafield*>);

    void setSettingsSelectedCategory(int);
    void setSettingsSelectedCustomfield(int);
};

#endif // SETTINGS_H
