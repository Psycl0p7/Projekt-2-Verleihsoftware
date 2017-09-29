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
    QObject::connect(this, SIGNAL(do_loadCustomFields()), this , SLOT(loadCustomFields()));

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

    this->geraeteTypenReady = false;
    if(!this->dbHandler.geratetypenAuslesen(&qry, &error))
        QMessageBox::warning(this, "Fehler", "Geraetetypen konnten nicht ausgelesen werden: " + error);
    else
    {
        this->ui->cb_typ->clear();
        while(qry.next())
            this->ui->cb_typ->addItem(qry.value(0).toString());

        this->geraeteTypenReady = true;
        emit this->do_loadCustomFields();
    }
}

void MainWindow::loadCustomFields() {
    QString current = this->ui->cb_typ->currentText();
    if(current.isEmpty())
        return;

    QSqlQuery p_qry;
    QString error;
    if(!this->dbHandler.getCustomFelder(&p_qry, &error, current))
        QMessageBox::warning(this, "Fehler", "Auslesen der CustomFelder nicht möglich: " + error);
    else
    {
        this->ui->cb_datenfeld->clear();
        while(p_qry.next())
            this->ui->cb_datenfeld->addItem(p_qry.value(0).toString());
        this->ui->cb_datenfeld->addItem(QString("Neu anlegen"));
    }
}

void MainWindow::on_cb_typ_currentIndexChanged(const QString &arg1)
{
    if(this->geraeteTypenReady)
        emit this->do_loadCustomFields();
}

void MainWindow::createNewCustomField()
{
    if(this->ui->edt_datenfeldBezeichnung->text().isEmpty()) {
        QMessageBox::information(this, "Information", "Bitte Namen angeben.");
        return;
    }

    QString error;
    QString name = this->ui->edt_datenfeldBezeichnung->text();
    QString datentyp = this->ui->cb_datenfeldTyp->currentText();
    QString geraetetyp = this->ui->cb_typ->currentText();
    bool pflichtfeld = this->ui->cb_pflichtfeld->isChecked();

    if(!this->dbHandler.createNewCustomField(&error,name,geraetetyp,datentyp,pflichtfeld))
        QMessageBox::warning(this, "Fehler", "Feld konnte nicht angelegt werden: " + error);
    else
    {
        emit do_loadCustomFields();
        QMessageBox::information(this, "Information", "Datenfeld wurde angelegt");
    }
}

void MainWindow::saveCustomField()
{
    QString error;
    QString name = this->ui->edt_datenfeldBezeichnung->text();
    QString datentyp = this->ui->cb_datenfeldTyp->currentText();
    QString geraetetyp = this->ui->cb_typ->currentText();
    bool pflichtfeld = this->ui->cb_pflichtfeld->isChecked();

    if(!this->dbHandler.saveCustomField(name,geraetetyp, datentyp,pflichtfeld, &error))
        QMessageBox::warning(this, "Fehler", "Datenfeld konnte nicht geändert werden: " + error);
    else {
        if(!this->dbHandler.loadCustomField(geraetetyp, &name, &datentyp, &pflichtfeld))
            QMessageBox::warning(this,"Fehler", "Datenfeld konnte nicht neu geladen werden: " + error);
        else {
            this->ui->edt_datenfeldBezeichnung->setText(name);
            this->ui->cb_datenfeldTyp->setCurrentText(datentyp);
            this->ui->cb_pflichtfeld->setChecked(pflichtfeld);
        }
    }
}


void MainWindow::on_btn_saveCustomField_clicked()
{
    if(this->ui->cb_datenfeld->currentText() == "Neu anlegen")
        this->createNewCustomField();
    else
        this->saveCustomField();
}
