#ifndef DATAFIELD_H
#define DATAFIELD_H
#include <QString>

class Datafield
{
public:
    static const int TYPE_TEXT = 0;

    Datafield(QString name, int type, QString data = NULL);
    QString getName();
    int getType();
    QString getData();
    void setData(QString data);
private:
    QString name;
    QString data;
    int type;

};

#endif // DATAFIELD_H
