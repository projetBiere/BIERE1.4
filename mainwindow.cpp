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
#include <QtConcurrent/QtConcurrent>
#include <QPainter>
#include <QFuture>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dateToday = QDate::currentDate();
    timeToday = QTime::currentTime();
    strTime = timeToday.toString("hh_mm_ss");
    strDate = dateToday.toString("yyyy_MM_dd");
    outputDate = strDate + "__" + strTime;

    model = new QStandardItemModel(0,0, this);

    QSettings settings("Alan Simonin", "BIERE");

    optLength = settings.value("Longueur par défaut", 170).toInt();
    optEvalueUnit = settings.value("Evalue unités par défaut", 4).toInt();
    optEvalueExp = settings.value("Evalue exp par défaut", 50).toInt();
    optDossierDeTravail = settings.value("Dossier de travail", QDir::homePath()).toString();

    ui->spinLength->setValue(optLength);
    ui->spinEvalueUnit->setValue(optEvalueUnit);
    ui->spinEvalueExp->setValue(optEvalueExp);

    connect(this, SIGNAL(texteFichierAChange(int,QString)), this, SLOT(changerTexteFichier(int,QString)));
    connect(this, SIGNAL(parsingTermine()), this, SLOT(voirSiTousLesParsingOntTermine()));
    connect(this, SIGNAL(parsingTermines()), this, SLOT(lesParsingOntTermine()));
    connect(this, SIGNAL(texteStatusBarChange(QString)), this, SLOT(changerTexteStatusBar(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changerTexteStatusBar(QString newMessage)
{
    statusBar()->showMessage(newMessage);
}

void MainWindow::changerPourcentageModele(QString newPourcentage, int numLigne)
{
    model->item(numLigne,1)->setText(newPourcentage);
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::lesThreadOntTermine()
{
    ui->boutonTestLength->setText(tr("Test !"));
}

void MainWindow::lesParsingOntTermine()
{
    ui->boutonEnleverFichiers->setEnabled(true);
    cheminOutput = ui->lineDossier->text() + "/output/" + outputDate + "/";
    cheminTemp = ui->lineDossier->text() + "/output/" + outputDate + "/temp/";
    for(int i=0; i<model->rowCount(); i++)
    {
        QString nomCompletPartieMatrice = cheminTemp + model->item(i,0)->text() + "_matrice_de_sortie.txt";
        QFile partieMatrice(nomCompletPartieMatrice);
        QString nomCompletMatriceDeSortie = cheminOutput + "matrice_de_sortie.txt";
        QFile matriceDeSortie(nomCompletMatriceDeSortie);
        QTextStream outMatrice(&matriceDeSortie);

        QString nomCompletPartieMatriceDeCompte = cheminTemp + model->item(i,0)->text() + "_matrice_de_compte.txt";
        QFile partieCompte(nomCompletPartieMatriceDeCompte);
        QTextStream outPartieCompte(&partieCompte);
        QMap<QString, float>  mapCompte;
        QMap<QString, float>::Iterator iterateur;
        QString nomCompletMatriceDeCompte = cheminOutput + "matrice_de_compte.txt";
        QFile matriceDeCompte(nomCompletMatriceDeCompte);
        QTextStream outCompte(&matriceDeCompte);

        if(!partieMatrice.open(QIODevice::ReadOnly | QIODevice::Text))
            qDebug() << tr("Erreur d'ouverture du fichier...");

        if(!matriceDeSortie.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            qDebug() << tr("Erreur d'ouverture du fichier...");

        if(!partieCompte.open(QIODevice::WriteOnly | QIODevice::Text))
            qDebug() << tr("Erreur d'ouverture du fichier...");

        if(!matriceDeCompte.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            qDebug() << tr("Erreur d'ouverture du fichier...");

        emit texteStatusBarChange(tr("Fusion des fichiers output..."));

        QString tempQueryDescription = "";
        QStringList tabMatch;
        int nbreMatch = 0;

        while(!partieMatrice.atEnd())
        {
            QString strLigne = partieMatrice.readLine();            

            outMatrice << strLigne;

            QStringList listLigne = strLigne.split("\t\t");            

            if(listLigne[1] == tempQueryDescription)
            {
                nbreMatch++;
                tabMatch << listLigne[2].simplified();
            }
            else
            {
                if(tempQueryDescription.isEmpty())
                {
                    tempQueryDescription = listLigne[1];
                    nbreMatch++;
                    tabMatch << listLigne[2].simplified();
                }
                else
                {
                    for(int i = 0; i < tabMatch.length(); i++)
                    {
                        if(mapCompte.contains(tabMatch[i]))
                            mapCompte[tabMatch[i]] += (float)1/nbreMatch;
                        else
                            mapCompte.insert(tabMatch[i], (float)1/nbreMatch);
                    }
                    tempQueryDescription = listLigne[1];
                    nbreMatch = 1;
                    tabMatch.clear();
                    tabMatch << listLigne[2].simplified();
                }
            }
        }

        for(iterateur = mapCompte.begin(); iterateur != mapCompte.end(); ++iterateur)
        {
            outPartieCompte << model->item(i,0)->text() << "\t\t" << iterateur.value() << "\t\t" << iterateur.key() << endl;
            outCompte << model->item(i,0)->text() << "\t\t" << iterateur.value() << "\t\t" << iterateur.key() << endl;
        }

        partieMatrice.close();
        matriceDeSortie.close();
        partieCompte.close();
        matriceDeCompte.close();
    }
    emit texteStatusBarChange(tr("Fusion des fichiers output... Terminé !"));
}

void MainWindow::voirSiTousLesThreadsOntTermine()
{
    int nombreThreads = 0;
    nombreThreads++;
    if(nombreThreads == model->rowCount())
    {
        emit threadsTermines();
        nombreThreads = 0;
    }
}

int nombreThreads = 0;
void MainWindow::voirSiTousLesParsingOntTermine()
{

    nombreThreads++;
    if(nombreThreads == model->rowCount())
    {
        emit parsingTermines();
        nombreThreads = 0;
    }
}

void MainWindow::changerTexteFichier(int numLigne, QString newTexte)
{
    model->item(numLigne,1)->setText(newTexte);
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::extraireEchantillons(QString nomFichier, int numLigne, QVector<int> seuils)
{
    cheminOutput = ui->lineDossier->text() + "/output/" + outputDate + "/";
    cheminTemp = ui->lineDossier->text() + "/output/" + outputDate + "/temp/";
    QDir dossierDeTravail(ui->lineDossier->text() + "/");
    dossierDeTravail.mkpath("output/" + outputDate + "/temp/");
    QFile fichierALire(ui->lineDossier->text() + "/" + model->item(numLigne,0)->text());
    QFile matriceDeSortie(cheminTemp + nomFichier + "_matrice_de_sortie.txt");

    if(!fichierALire.open(QFile::ReadOnly | QFile::Text))   // s'il y a une erreur d'ouverture du fichier
        qDebug() << tr("Erreur d'ouverture du fichier à lire...");

    if(!matriceDeSortie.open(QIODevice::WriteOnly | QIODevice::Text)) //  | QIODevice::Append
        qDebug() << tr("Erreur d'ouverture du fichier matrice de sortie...");

    QTextStream out(&matriceDeSortie);

    bool regarderLigneSuivante = false;
    int iterateur = 0;
    int elementsAjoutes = 0;
    char typeDeFichier = NULL;
    QString pourcentageParsing;
    QString queryDescription;
    QString NomSeqDB;
    QStringList echantillon; // query description, length, score, expect, identities, gaps, nomseqdb
    int nbreMatch = 0;
    for(int i=0; i<7; i++)
    {
        echantillon.append("");
    }

    qint64 tailleParsee = 0;
    qint64 tailleFichier = fichierALire.size();
    pourcentageParsing = "0%";
    emit texteFichierAChange(numLigne, pourcentageParsing);
    QRegExp regSwipe("SWIPE*");
    QRegExp regBlast("BLAST*");

    while(!fichierALire.atEnd())
    {
        QString ligne = fichierALire.readLine();
        if(ligne.contains(regSwipe))
            typeDeFichier = 'S';
        else if(ligne.contains(regBlast))
            typeDeFichier = 'B';

        tailleParsee += ligne.size();
        iterateur++;

        if(pourcentageParsing.compare(QString::number(100*tailleParsee / tailleFichier) + "%") != 0)
        {
            pourcentageParsing = QString::number(100*tailleParsee / tailleFichier) + "%";
            emit texteFichierAChange(numLigne, pourcentageParsing);
        }

        if(regarderLigneSuivante)                           // cette boucle permet de lire les nomseqdb sur plusieurs lignes
        {
            if(ligne.trimmed().startsWith("Length"))        // si la ligne commence par "Length"...
            {
                regarderLigneSuivante = false;              // on arrête la boucle
                echantillon[6] = NomSeqDB.simplified();
                QStringList liste = ligne.split("=");       // extraction de la length
                echantillon[1] = liste[1].trimmed();
            }
            else                                            // ...sinon on rajoute cette ligne aux précédentes
            {
                NomSeqDB += ligne.simplified();
            }
        }

        if(ligne.contains("Query description") && typeDeFichier == 'S')     // extraction de la query description
        {
            QStringList liste = ligne.split(": ");                          // on splite la ligne par rapport à ": " et non ":" !
            queryDescription = liste[1].trimmed();

            echantillon[0] = queryDescription;
        }
        else if(ligne.contains("Query=") && typeDeFichier == 'B')           // extraction de la query description
        {
            QStringList liste = ligne.split("= ");                          // on splite la ligne par rapport à ": " et non ":" !
            queryDescription = liste[1].trimmed();

            echantillon[0] = queryDescription;
        }
        else if(ligne.contains("Expect"))                   // extraction de score et expect
        {
            QStringList liste = ligne.split("=");           // on splite par rapport aux "=" et on stocke le résultat dans "liste"
            QStringList score = liste[1].split(",");        // on splite le deuxième élément de "liste" par rapport à "," que l'on stocke dans "score"

            echantillon[2] = score[0].trimmed();
            echantillon[3] = liste[2].trimmed();
        }
        else if(ligne.contains("Identities"))               // extraction de identities et gaps
        {
            QStringList liste = ligne.split("=");           // on splite la ligne par rapport aux "=" et on stocke le résultat dans "liste"

            QStringList identities = liste[1].split(",");   // on splite le deuxième élément de "liste" par rapport à "," que l'on stocke dans "identities"

            echantillon[4] = identities[0];

            if(liste.size() > 2)                            // si "liste" contient au moins 3 éléments, c'est qu'il y a un gap
            {
                echantillon[5] = liste[2];
            }
            else
            {
                echantillon[5] = "";
            }

            int lengthEchantillon = echantillon[1].toInt();
            if(lengthEchantillon >= seuils[0])                 // test de la Length
            {
                if(echantillon[3].contains("e-"))               // test de la evalue (seulement si elle est donnée sous le format (a)e-(b)
                {
                    QStringList listeEvalue = echantillon[3].split("e-");
                    int evalueUnitEchantillon = listeEvalue[0].toInt();
                    int evalueExpEchantillon = listeEvalue[1].toInt();
                    if(evalueExpEchantillon > seuils[2] || (evalueExpEchantillon == seuils[2] && evalueUnitEchantillon <= seuils[1]))
                    {
                        int scoreEchantillon = echantillon[2].toInt();
                        qDebug() << echantillon[2];
                        //if()
                            QStringList listeTaxo = echantillon[6].split("|");
                            elementsAjoutes++;
                            nbreMatch++;

                            out << nomFichier << "\t\t" << echantillon[0] << "\t\t" << echantillon[6] << endl;

                            listeTaxo.pop_front();      // ces trois lignes suppriment le trois premiers éléments de listeTaxo
                            listeTaxo.pop_front();      // (>gnl|BL_ORD_ID|10319 AF164130.1.1775_U par exemple)
                            listeTaxo.pop_front();
                    }
                    else{}
                        //qDebug() << "evalue trop grande ! (" << echantillon[3] << " au lieu de " << this->mSeuilExpUnit << "e-" << this->mSeuilExpPuiss << ")";
                }
                else {}
                    //qDebug() << "evalue trop grande ! (" << echantillon[3] << " au lieu de " << this->mSeuilExpUnit << "e-" << this->mSeuilExpPuiss << ")";
            }
            else{}
                    //qDebug() << "length trop petite ! (" << echantillon[1] << " au lieu de " << this->mSeuilLength <<  ")";

        }
        else if(ligne.startsWith(">"))                      // extraction de nomseqdb
        {
            regarderLigneSuivante = true;                   // il faudra regarder les lignes suivantes
            NomSeqDB = ligne.simplified().remove(0,1);      // et on stocke la ligne dans NomSeqDB
        }
    }
    emit changerTexteFichier(numLigne, pourcentageParsing);

    fichierALire.close();
    matriceDeSortie.close();

    emit texteFichierAChange(numLigne, "Terminé");
    emit parsingTermine();
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

void MainWindow::on_boutonParse_clicked()
{
    ui->boutonEnleverFichiers->setEnabled(false);
    emit changerTexteStatusBar("");
    QStringList header;
    ui->tableView->showColumn(1);

    header << tr("Nom de fichier") << tr("Parsing");
    model->setHorizontalHeaderLabels(header);

    for(int i=0; i<model->rowCount(); i++)
    {
        QVector<int> seuilsEchantillon;
        seuilsEchantillon.resize(4);
        seuilsEchantillon[0] = ui->spinLength->value();
        seuilsEchantillon[1] = ui->spinEvalueUnit->value();
        seuilsEchantillon[2] = ui->spinEvalueExp->value();
        seuilsEchantillon[3] = ui->spinScore->value();
        QtConcurrent::run(this, &MainWindow::extraireEchantillons, model->item(i,0)->text(), i, seuilsEchantillon);
    }
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::on_boutonAjouterFichiers_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Fichiers à traiter"), optDossierDeTravail, tr("Fichiers BDM (BDM*);;Tous les fichiers (*.*)"));
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
    model->clear();
}

void MainWindow::on_boutonQuitter_clicked()
{
    QSettings settings("Alan Simonin", "BIERE");

    settings.setValue("Longueur par défaut", ui->spinLength->value());
    settings.setValue("Evalue unités par défaut", ui->spinEvalueUnit->value());
    settings.setValue("Evalue exp par défaut", ui->spinEvalueExp->value());
    if(ui->lineDossier->text().isEmpty())
        settings.setValue("Dossier de travail", optDossierDeTravail);
    else
        settings.setValue("Dossier de travail", ui->lineDossier->text());

    QCoreApplication::quit();
}

void MainWindow::closeEvent(QEvent *event)
{
    QSettings settings("Alan Simonin", "BIERE");

    settings.setValue("Longueur par défaut", ui->spinLength->value());
    settings.setValue("Evalue unités par défaut", ui->spinEvalueUnit->value());
    settings.setValue("Evalue exp par défaut", ui->spinEvalueExp->value());
    settings.setValue("Dossier de travail", ui->lineDossier->text());

    qDebug() << "Clic sur croix ";

    event->accept();
}

void MainWindow::on_actionA_propos_triggered()
{
    QMessageBox::about(this, tr("A propos de BIERE"),
                       "Version 1.2<br/><br/>"
                       "<a rel=\"license\" href=\"http://creativecommons.org/licenses/by-sa/3.0/ch/deed.fr\"></a>"
                       "<br />Ce(tte) œuvre est mise à disposition selon les termes de la "
                       "<a rel=\"license\" href=\"http://creativecommons.org/licenses/by-sa/3.0/ch/deed.fr\">"
                       "Licence Creative Commons Attribution -  Partage dans les Mêmes Conditions 3.0 Suisse</a>."
                       "<br/><br/><a href=\"https://sourceforge.net/projects/biere/\">Voici le lien SourceForge de ce projet.</a>");
}

void MainWindow::histoEvalue(int numLigne)
{
    //QVector<float> tabValeurs(100);
    //float tabValeurs[100];
    for(int i=0; i<model->rowCount(); i++)
        for(int j=0; j<100; j++)
            tabValeurs[i][j] = 0;
    QString pourcentageParsing;
    QFile fichierALire(ui->lineDossier->text() + "/" + model->item(numLigne,0)->text());
    qint64 tailleParsee = 0;
    qint64 tailleFichier = fichierALire.size();
    QRegExp evalue0("\[1-9]\\.\\d{2}");
    QRegExp evalue1("0\\.\[1-9]\\d");
    QRegExp evalue2("0\\.0\[1-9]");
    QRegExp evalue3("0\\.00\\d");

    if(!fichierALire.open(QFile::ReadOnly | QFile::Text))   // s'il y a une erreur d'ouverture du fichier
        qDebug() << tr("Erreur d'ouverture du fichier à lire...");

    while(!fichierALire.atEnd())
    {
        QString ligne = fichierALire.readLine();
        tailleParsee += ligne.size();

        if(pourcentageParsing.compare(QString::number(100*tailleParsee / tailleFichier) + "%") != 0)
        {
            pourcentageParsing = QString::number(100*tailleParsee / tailleFichier) + "%";
            emit texteFichierAChange(numLigne, pourcentageParsing);
        }

        if(ligne.contains("Expect"))                   // extraction de score et expect
        {
            QStringList liste = ligne.split("=");

            QString eValue = liste[2].trimmed();
            if(eValue.contains("e-"))
            {
                QStringList listeEvalue = eValue.split("e-");

                int puissanceEvalue = listeEvalue[1].toInt();

                tabValeurs[numLigne][puissanceEvalue]++;
            }
            else if(eValue.contains(evalue0))
            {
                tabValeurs[numLigne][0]++;
            }
            else if(eValue.contains(evalue1))
            {
                tabValeurs[numLigne][1]++;
            }
            else if(eValue.contains(evalue2))
            {
                tabValeurs[numLigne][2]++;
            }
            else if(eValue.contains(evalue3))
            {
                tabValeurs[numLigne][3]++;
            }
            tailleParsee++;
        }
    }

    emit texteFichierAChange(numLigne, "Terminé");
    ui->boutonEnleverFichiers->setEnabled(true);

    fichierALire.close();
}

void MainWindow::on_boutonHisto_clicked()
{
    if(model->rowCount() != 0)
    {
        ui->boutonEnleverFichiers->setEnabled(false);
        emit changerTexteStatusBar("");
        QStringList header;
        ui->tableView->showColumn(1);

        QFutureWatcher<void> *watcher[model->rowCount()];

        header << tr("Nom de fichier") << tr("Parsing e-value");
        model->setHorizontalHeaderLabels(header);

        tabValeurs.resize(model->rowCount());

        QSignalMapper *signalMapper = new QSignalMapper(this);
        connect(signalMapper, SIGNAL(mapped(int)), this, SIGNAL(parsingHistoTermine(int)));
        connect(this, SIGNAL(parsingHistoTermine(int)), this, SLOT(afficherHistogramme(int)));

        for(int i=0; i<model->rowCount(); i++)
        {
            tabValeurs[i].resize(100);

            watcher[i] = new QFutureWatcher<void>;

            signalMapper->setMapping(watcher[i], i);

            connect(watcher[i], SIGNAL(finished()), signalMapper, SLOT(map()));
            connect(watcher[i], SIGNAL(finished()), watcher[i], SLOT(deleteLater()));

            QFuture<void> future = QtConcurrent::run(this, &MainWindow::histoEvalue, i);
            watcher[i]->setFuture(future);
        }
        ui->tableView->resizeColumnsToContents();
    }
}

void MainWindow::afficherHistogramme(int numero)
{
    //qDebug() << numero;
    Histogramme *histo = new Histogramme(model->item(numero,0)->text(), tabValeurs[numero]);
    histo->setWindowModality(Qt::WindowModal);
    histo->exec();
    //4histo.deleteLater();
}
