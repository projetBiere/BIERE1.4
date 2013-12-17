#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QStandardItemModel>
#include <QDir>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel(0,0, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changerTexteStatusBar(QString newMessage)
{
    statusBar()->showMessage(newMessage);
}

void MainWindow::changerTextePourcentage(QString newMessage)
{
    ui->labelPourcentage->setText(newMessage);
}

void MainWindow::changerPourcentageModele(QString newPourcentage, int numLigne)
{
    model->item(numLigne,1)->setText(newPourcentage);
}

void MainWindow::lesThreadOntTermine()
{
    ui->boutonTestLength->setText(tr("Test !"));
}

void MainWindow::tailleAutoDesColonnes(QString rien, int zero)
{
    ui->tableView->resizeColumnsToContents();
}

int nombreThreads = 0;
void MainWindow::voirSiTousLesThreadsOntTermine()
{
    nombreThreads++;
    if(nombreThreads == model->rowCount())
        emit threadsTermines();
}

void MainWindow::on_boutonTestLength_clicked()
{
    threadTest* listeThread[model->rowCount()];
    int valeurLength = ui->spinLength->value();
    int valeurEvalueUnites = ui->spinEvalueUnit->value();
    int valeurEvalueExp = ui->spinEvalueExp->value();
    QStringList header;
    header << tr("Nom de fichier") << tr("Résultat du test");
    model->setHorizontalHeaderLabels(header);

    if(!ui->checkBoxEvalue->isChecked())
    {
        valeurEvalueUnites = -1;
        valeurEvalueExp = -1;
    }

    if(!ui->checkBoxLength->isChecked())
        valeurLength = -1;

    for(int i = 0; i < model->rowCount(); i++)
    {
        QString nomCompletFichier = ui->lineDossier->text() + "/" + model->item(i,0)->text();

        listeThread[i] = new threadTest(nomCompletFichier, valeurLength, valeurEvalueUnites, valeurEvalueExp, i);

        connect(listeThread[i], SIGNAL(pourcentageAChange(QString, int)), this, SLOT(changerPourcentageModele(QString, int)));
        connect(listeThread[i], SIGNAL(pourcentageAChange(QString, int)), this, SLOT(tailleAutoDesColonnes(QString,int)));
        connect(this, SIGNAL(clicSurStop()), listeThread[i], SLOT(stopperThread()));
        connect(listeThread[i], SIGNAL(threadTermine()), this, SLOT(voirSiTousLesThreadsOntTermine()));
        connect(this, SIGNAL(threadsTermines()), this, SLOT(lesThreadOntTermine()));
        connect(listeThread[i], SIGNAL(finished()), listeThread[i], SLOT(deleteLater()));
    }

    if(ui->boutonTestLength->text() == tr("Stop"))
    {
        emit clicSurStop();
        for(int i = 0; i < model->rowCount(); i++)
        {
            delete listeThread[i];
        }
        ui->boutonTestLength->setText(tr("Test !"));
    }
    else
    {
        ui->tableView->showColumn(1);
        for(int j=0; j<model->rowCount(); j++)
        {
            listeThread[j]->start();
        }
        ui->boutonTestLength->setText(tr("Stop"));
    }
}

QStringList listeFenetresOuvertes;
void MainWindow::on_boutonOk_clicked()
{
    threadParsing* listeThread[model->rowCount()];
    fenetreResultats *listeFenetres[model->rowCount()];
    QStringList header;
    int valeurLength = ui->spinLength->value();
    int valeurEvalueUnites = ui->spinEvalueUnit->value();
    int valeurEvalueExp = ui->spinEvalueExp->value();

    for(int i = 0; i<model->rowCount(); i++)
    {
        QString nomCompletFichier = ui->lineDossier->text() + "/" + model->item(i,0)->text();
        model->item(i,1)->setText(tr("Calcul du nombre de lignes..."));
        listeFenetres[i] = new fenetreResultats(this);
        if(!listeFenetresOuvertes.contains(model->item(i,0)->text()))
        {
            listeFenetresOuvertes << model->item(i,0)->text();

            if(!ui->checkBoxEvalue->isChecked())
            {
                valeurEvalueUnites = -1;
                valeurEvalueExp = -1;
            }

            if(!ui->checkBoxLength->isChecked())
                valeurLength = -1;

            listeThread[i] = new threadParsing(nomCompletFichier, valeurLength, valeurEvalueUnites, valeurEvalueExp, i, listeFenetres[i]);
            listeFenetres[i]->setWindowTitle(model->item(i,0)->text());

            listeFenetres[i]->show();
            listeThread[i]->start();
        }
    }
    header << tr("Nom de fichier") << tr("Parsing");
    model->setHorizontalHeaderLabels(header);
    ui->tableView->showColumn(1);
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::on_boutonAjouterFichiers_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Fichiers à traiter"), "C:/copies", tr("Fichiers BDM (BDM*);;Tous les fichiers (*.*)"));
    if(!fileNames.isEmpty())
    {
        QFileInfo dossier = fileNames[0];
        QList<QStandardItem *> listeItem;
        QList<QStandardItem *> listeNbres;
        QStringList header;
        header << tr("Nom de fichier");

        for(int i = 0; i<fileNames.length(); i++)
        {
            QFileInfo cheminFichier = fileNames[i];
            QStandardItem *item = new QStandardItem(cheminFichier.fileName());
            QStandardItem *nbre = new QStandardItem("");
            nbre->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
            listeItem << item;
            listeNbres << nbre;
        }

        model->clear();

        model->appendColumn(listeItem);
        model->appendColumn(listeNbres);
        model->setHorizontalHeaderLabels(header);

        ui->tableView->verticalHeader()->hide();
        ui->tableView->setModel(model);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setShowGrid(false);
        ui->tableView->hideColumn(1);
        ui->lineDossier->setText(dossier.dir().absolutePath());
    }
}

void MainWindow::on_boutonEnleverFichiers_clicked()
{
    listeFenetresOuvertes.clear();
    model->clear();
}
