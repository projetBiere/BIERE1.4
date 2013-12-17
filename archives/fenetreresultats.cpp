#include "fenetreresultats.h"
#include "ui_fenetreresultats.h"
#include <QDebug>
#include <QTableWidget>

fenetreResultats::fenetreResultats(QWidget *parent) : QMainWindow(parent), ui(new Ui::fenetreResultats)
{
    ui->setupUi(this);

    modele = new QStandardItemModel;

    ui->tableWidget->setColumnCount(8);
    QStringList header;
    for(int i=0; i<8; i++)
        header << QString::number(i);
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setRowCount(1);
    iter = 0;
}

fenetreResultats::~fenetreResultats()
{
    delete ui;
}

void fenetreResultats::setMessageStatusBar(QString texte)
{
    ui->statusbar->showMessage(texte);
}

void fenetreResultats::ajouterLigne(QStringList liste)
{
    ui->tableWidget->setRowCount(this->iter+2);
    for(int j=0; j<8; j++)
    {
        ui->tableWidget->setItem(this->iter, j, new QTableWidgetItem(liste[j]));
    }
    ui->tableWidget->resizeColumnsToContents();
    this->iter++;
}
