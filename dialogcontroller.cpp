#include "dialogcontroller.h"

DialogController::DialogController()
{

}

void DialogController::showWarning(QString warning, QString error)
{
    emit this->si_showWarning(warning, error);
}

void DialogController::showInformation(QString information)
{
    emit this->si_showInformation(information);
}
