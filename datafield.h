#ifndef DATAFIELD_H
#define DATAFIELD_H
#include <QString>

class Datafield
{
public:
    static const int TYPE_UNDEFINED = -1;
    static const int TYPE_TEXT = 0;

    Datafield(QString name, int type = Datafield::TYPE_UNDEFINED, bool required = false, QString data = NULL);
    Datafield(QString name, QString data);
    QString getName();
    int getType();
    bool isRequired();
    QString getData();
    void setData(QString data);
    void updateMeta(QString name, int type, bool required);
private:
    QString name;
    QString data;
    int type;
    bool required;

};

#endif // DATAFIELD_H
