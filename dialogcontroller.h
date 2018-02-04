#ifndef DIALOGCONTROLLER_H
#define DIALOGCONTROLLER_H

#include <QString>
#include <QObject>

class DialogController : public QObject
{
    Q_OBJECT

public:
    DialogController();
    void showWarning(QString warning, QString error);
    void showInformation(QString information);
signals:
    void si_showWarning(QString, QString);
    void si_showInformation(QString);
};

#endif // DIALOGCONTROLLER_H
