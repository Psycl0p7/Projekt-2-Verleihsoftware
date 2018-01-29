#ifndef DATAFIELD_H
#define DATAFIELD_H
#include <QString>

class Datafield
{
public:
    Datafield(QString name, int type, QString data);
    QString getName();
    int getType();
    QString getData();
    void setData(QString data);
private:
    QString name;
    QString data;
    int type;

    static const int TYPE_STRING = 0;
    static const int TYPE_BOOL = 1;
    static const int TYPE_PATH = 2;

};

#endif // DATAFIELD_H
