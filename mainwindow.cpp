#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    this->ui->cb_datenfeld->addItem(QString("Neu anlegen"));
    this->gereateTypenEinlesen();
    this->feldDatentypenEinlesen();
}

void MainWindow::feldDatentypenEinlesen()
{
    QSqlQuery qry;
    QString error;
    if(!this->dbHandler.feldDatentypenAuslesen(&qry,&error))
        QMessageBox::warning(this,"Fehler", "Datentypen konnten nicht gelsen werden: " + error);
    else
    {   this->ui->cb_datenfeldTyp->clear();
        while(qry.next())
            this->ui->cb_datenfeldTyp->addItem(qry.value(0).toString());
    }
}

void MainWindow::on_btn_neuenTypAnlegen_clicked()
{
    if(this->ui->edt_neuerGereateTypName->text().length() <= 0)
    {
        QMessageBox::information(this,"Information", "Bitte Namen angeben.");
        return;
    }

    QString error = "";
    if(this->dbHandler.geratetypAnlegen(this->ui->edt_neuerGereateTypName->text(), &error))
        QMessageBox::information(this, "Information", "Geraetetyp wurde angelegt.");
    else
        QMessageBox::warning(this, "Fehler", error);

    this->ui->edt_neuerGereateTypName->clear();
    this->gereateTypenEinlesen();
}


void MainWindow::gereateTypenEinlesen()
{
    QSqlQuery qry;
    QString error;

    if(!this->dbHandler.geratetypenAuslesen(&qry, &error))
        QMessageBox::warning(this, "Fehler", "Geraetetypen konnten nicht ausgelesen werden: " + error);
    else
    {
        this->ui->cb_typ->clear();
        while(qry.next())
            this->ui->cb_typ->addItem(qry.value(0).toString());
    }
}

void MainWindow::on_cb_typ_currentIndexChanged(const QString &arg1) // Geraetetypen namen auslelsen
{
    if(arg1 == "Neu anlegen")
        return;

    QSqlQuery p_qry;
    QString error;
    if(!this->dbHandler.getCustomFelder(&p_qry, &error, arg1))
        QMessageBox::warning(this, "Fehler", "Auslesen der CustomFelder nicht möglich: " + error);
    else
    {
        this->ui->cb_datenfeld->clear();
        while(p_qry.next())
            this->ui->cb_datenfeld->addItem(p_qry.value(0).toString());
        this->ui->cb_datenfeld->addItem(QString("Neu anlegen"));
    }
}
