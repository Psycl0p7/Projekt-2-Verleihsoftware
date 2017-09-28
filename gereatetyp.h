#ifndef GEREATETYP_H
#define GEREATETYP_H
#include <QString>

class Gereatetyp
{
public:
    Gereatetyp(int id, QString name);
    Gereatetyp();
    QString getName();
    int getId();
private:
    QString name;
    int id;
};

#endif // GEREATETYP_H
