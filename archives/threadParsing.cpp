#include "threadParsing.h"

threadParsing::threadParsing(QObject *parent) : QThread(parent) {}

threadParsing::threadParsing(QString nomFichierAParser, int seuilLength, int seuilExpUnit, int seuilExpPuiss, int numLigne, fenetreResultats *fenetre)
{
    mNomFichierAParser = nomFichierAParser;
    mSeuilLength = seuilLength;
    mSeuilExpUnit = seuilExpUnit;
    mSeuilExpPuiss = seuilExpPuiss;
    mNumLigne = numLigne;
    mFenetre = fenetre;
    modele = new QStandardItemModel(this);
    connect(this, SIGNAL(changerTexteStatusBar(QString)), fenetre, SLOT(setMessageStatusBar(QString)));
    connect(this, SIGNAL(ajouterLigneAuModele(QStringList)), this->mFenetre, SLOT(ajouterLigne(QStringList)));
}

QList< QList<QString> > listeEchantillonsOk;

void threadParsing::run()
{
    this->mATermine = false;
    this->setPriority(QThread::IdlePriority);

    emit this->threadRunning();

    QFile fichierALire(this->mNomFichierAParser);    
    QFile matriceDeSortie("c:/copies/matriceDeSortie.txt");
    QFile listeGlobalseqdb("c:/copies/listeGlobalseqdb.txt");

    if(!fichierALire.open(QFile::ReadOnly | QFile::Text))   // s'il y a une erreur d'ouverture du fichier
    {
        qDebug() << tr("Erreur d'ouverture du fichier...");
    }

    if(!matriceDeSortie.open(QIODevice::WriteOnly | QIODevice::Text)) //  | QIODevice::Append
        return;

    if(!listeGlobalseqdb.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&matriceDeSortie);
    out << "Echantillon\t\tNom seq fasta\t\tnomseqdb\n";

    bool regarderLigneSuivante = false;
    int iterateur = 0;
    this->mElementsAjoutes = 0;
    QString queryDescription;
    QString NomSeqDB;
    QList<QStandardItem *> echantillon; // query description, length, score, expect, identities, gaps, nomseqdb
    this->mGlobalseqdb = 0;
    for(int i=0; i<7; i++)
    {
        echantillon.append(new QStandardItem(""));
    }

    while(!fichierALire.atEnd() && this->mATermine == false)
    {
        QString ligne = fichierALire.readLine();

        if(regarderLigneSuivante)                           // cette boucle permet de lire les nomseqdb sur plusieurs lignes
        {
            if(ligne.trimmed().startsWith("Length"))        // si la ligne commence par "Length"...
            {
                regarderLigneSuivante = false;              // on arrête la boucle
                echantillon[6] = new QStandardItem(NomSeqDB.simplified());
            }
            else                                            // ...sinon on rajoute cette ligne aux précédentes
            {
                NomSeqDB += ligne.simplified();
            }
        }

        if(ligne.contains("Query description"))             // extraction de la query description
        {
            QStringList liste = ligne.split(": ");          // on splite la ligne par rapport à ": " et non ":" !
            queryDescription = liste[1].trimmed();

            echantillon[0] = new QStandardItem(queryDescription);
        }
        else if(ligne.contains("Length"))                   // extraction de la length
        {
            QStringList liste = ligne.split("=");           // on splite par rapport à "="

            echantillon[1] = new QStandardItem(liste[1].trimmed());
        }
        else if(ligne.contains("Expect"))                   // extraction de score et expect
        {
            QStringList liste = ligne.split("=");           // on splite par rapport aux "=" et on stocke le résultat dans "liste"
            QStringList score = liste[1].split(",");        // on splite le deuxième élément de "liste" par rapport à "," que l'on stocke dans "score"            

            echantillon[2] = new QStandardItem(score[0].trimmed());
            echantillon[3] = new QStandardItem(liste[2].trimmed());
        }
        else if(ligne.contains("Identities"))               // extraction de identities et gaps
        {
            QStringList liste = ligne.split("=");           // on splite la ligne par rapport aux "=" et on stocke le résultat dans "liste"

            QStringList identities = liste[1].split(",");   // on splite le deuxième élément de "liste" par rapport à "," que l'on stocke dans "identities"            

            echantillon[4] = new QStandardItem(identities[0]);

            if(liste.size() > 2)                            // si "liste" contient au moins 3 éléments, c'est qu'il y a un gap
            {                
                echantillon[5] = new QStandardItem(liste[2]);
            }
            else
            {
                echantillon[5] = new QStandardItem("");
            }

            int lengthEchantillon = echantillon[1]->text().toInt();
            if(lengthEchantillon >= this->mSeuilLength)                 // test de la Length
            {
                if(echantillon[3]->text().contains("e-"))               // test de la evalue (seulement si elle est donnée sous le format (a)e-(b)
                {
                    QStringList listeEvalue = echantillon[3]->text().split("e-");
                    int evalueUnitEchantillon = listeEvalue[0].toInt();
                    int evalueExpEchantillon = listeEvalue[1].toInt();
                    if(evalueExpEchantillon > this->mSeuilExpPuiss || (evalueExpEchantillon == this->mSeuilExpPuiss && evalueUnitEchantillon <= this->mSeuilExpUnit))
                    {
                        //listeEchantillonsOk << echantillon;

                        QStringList listeTaxo = echantillon[6]->text().split("|");

                        QString nomFichier = this->mNomFichierAParser.section("/",2);
                        out << nomFichier << "\t\t" << echantillon[0]->text() << "\t\t" << echantillon[6]->text() << endl;
                        out.flush();

                        listeTaxo.pop_front();      // ces trois lignes suppriment le trois premiers éléments de listeTaxo
                        listeTaxo.pop_front();      // (>gnl|BL_ORD_ID|10319 AF164130.1.1775_U par exemple)
                        listeTaxo.pop_front();

                        //emit ajouterLigneAuModele(listeTaxo);

                        this->mElementsAjoutes++;

                        /*if(elementsAjoutes%1000 == 0)
                        {*/
                            QString texte = QString::number(this->mElementsAjoutes) + " éléments ajoutés";
                            emit changerTexteStatusBar(texte);
                        //}
                        echantillon.erase(echantillon.begin()+1, echantillon.end());
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
            NomSeqDB = ligne.simplified();                  // et on stocke la ligne dans NomSeqDB
        }
        iterateur++;

    }
    this->mATermine = true;
    QString texte = QString::number(this->mElementsAjoutes) + " éléments ajoutés";
    emit changerTexteStatusBar(texte);
    this->mElementsAjoutes == 0 ? this->mGlobalseqdb = 0 : this->mGlobalseqdb = (float)1/this->mElementsAjoutes;

    QTextStream outListeGlobalseqdb(&listeGlobalseqdb);
    outListeGlobalseqdb << "Nom du fichier\t\tglobalseqdb\n";
    outListeGlobalseqdb << this->mNomFichierAParser.section("/",2) << "\t\t" << this->mGlobalseqdb << endl << flush;
    qDebug() << this->mNomFichierAParser << " : " << this->mElementsAjoutes << " et l'inverse est " << this->mGlobalseqdb;

    emit this->threadTermine();
}
