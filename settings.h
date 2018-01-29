#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVector>
#include "entry.h"
#include "dbhandler.h"

class Settings : public QObject
{
    Q_OBJECT

public:
    Settings(DBHandler* dbHandler);
    ~Settings();

    void init();
private:
    QVector<Entry*> categories;
    QVector<QString> supportedDatatypes;
    DBHandler* dbHandler;

    void readSupportedDatatypes();

    void getCategoryNames();
    void createCategory();
    void saveCategory();
    void deleteCategory(QString category);

    void getCustomFields();
    void createCustomfield();
    void saveCustomfield();
    void readCustomfield();
    void deleteCustomfield(QString category, QString fieldname);

signals:
    void showWarning(QString, QString);
    void showInformation(QString);

    void showSupportedTypes(QVector<QString>);
    void showCategories(QVector<Entry*>);
};

#endif // SETTINGS_H
