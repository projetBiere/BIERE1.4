#include "threadTest.h"
#include <QtCore>


threadTest::threadTest(QObject *parent) : QThread(parent) {}

threadTest::threadTest(QString nomFichierAParser, int seuilLength, int seuilExpUnit, int seuilExpPuiss, int numLigne)
{
    mNomFichierAParser = nomFichierAParser;
    mSeuilLength = seuilLength;
    mSeuilExpUnit = seuilExpUnit;
    mSeuilExpPuiss = seuilExpPuiss;
    mNumLigne = numLigne;
}

void threadTest::stopperThread()
{
    this->aTermine = true;
    this->exit(0);
    this->wait();
    deleteLater();
}

void threadTest::testLength(QString nomFichierAParser, int seuilLength, int numLigne)
{
    QFile fichierAParser(nomFichierAParser);
    double total = 0;
    double nombreOk = 0;
    QString pourcent;
    if(!fichierAParser.open(QIODevice::ReadOnly | QIODevice::Text))   // s'il y a une erreur d'ouverture du fichier
    {
        emit messageDErreurAChange(tr("Erreur d'ouverture du fichier..."));
        return;
    }

    while(!fichierAParser.atEnd() && !this->aTermine)
    {
        QString ligne = fichierAParser.readLine();
        if(ligne.trimmed().startsWith("Length"))
        {
            QStringList liste = ligne.split("=");           // on splite par rapport à "="
            QString strNombre = liste[1].trimmed();         // on garde juste le deuxième élément
            if(strNombre.toInt() >= seuilLength)
            {
                nombreOk++;
            }
            total++;
            pourcent = QString::number((int)(100*nombreOk / total)) + "% (" + QString::number(nombreOk) + "/" + QString::number(total) + ")";
            if((int)total%1000 == 0)
                emit pourcentageAChange(pourcent, numLigne);
        }
    }
    this->aTermine = true;
    emit pourcentageAChange(pourcent, this->mNumLigne);
}

void threadTest::testEvalue(QString nomFichierAParser, int seuilExpUnit, int seuilExpPuiss, int numLigne)
{
    QFile fichierAParser(nomFichierAParser);
    double total = 0;
    double nombreOk = 0;
    QString pourcent;
    if(!fichierAParser.open(QIODevice::ReadOnly | QIODevice::Text))   // s'il y a une erreur d'ouverture du fichier
    {
        emit messageDErreurAChange(tr("Erreur d'ouverture du fichier..."));
        return;
    }

    while(!fichierAParser.atEnd() && !this->aTermine)
    {
        QString ligne = fichierAParser.readLine();
        if(ligne.trimmed().startsWith("Score = "))
        {
            QStringList liste = ligne.split("=");           // on splite par rapport à "="
            QString strNombre = liste[2].trimmed();         // on garde juste le troisième élément
            QStringList strExpected = strNombre.split("e-");
            if(liste[2].contains("e-"))
            {
                int expectedUnites = strExpected[0].toInt();
                int expectedPuissance = strExpected[1].toInt();

                if(expectedPuissance > seuilExpPuiss)
                    nombreOk++;

                else if(expectedPuissance == seuilExpPuiss && expectedUnites <= seuilExpUnit)
                    nombreOk++;
            }
            total++;
            pourcent = QString::number((int)(100*nombreOk / total)) + "% (" + QString::number(nombreOk) + "/" + QString::number(total) + ")";
            if((int)total%1000 == 0)
                emit pourcentageAChange(pourcent, numLigne);
        }
    }
    this->aTermine = true;
    emit pourcentageAChange(pourcent, this->mNumLigne);
}

void threadTest::testLengthEvalue(QString nomFichierAParser, int seuilLength, int seuilExpUnit, int seuilExpPuiss, int numLigne)
{
    QFile fichierAParser(nomFichierAParser);
    bool longueurOk = false;
    double total = 0;
    double nombreOk = 0;
    QString pourcent;
    QStringList listeLength;
    QString strNombreLength;
    QStringList liste2;
    QString strNombre2;
    QStringList strExpected;
    if(!fichierAParser.open(QIODevice::ReadOnly | QIODevice::Text))   // s'il y a une erreur d'ouverture du fichier
    {
        emit messageDErreurAChange(tr("Erreur d'ouverture du fichier..."));
        return;
    }

    while(!fichierAParser.atEnd() && !this->aTermine)
    {
        QString ligne = fichierAParser.readLine();
        if(ligne.trimmed().startsWith("Length"))
        {
            listeLength = ligne.split("=");           // on splite par rapport à "="
            strNombreLength = listeLength[1].trimmed();         // on garde juste le deuxième élément
            if(strNombreLength.toInt() >= seuilLength)
            {
                longueurOk = true;
            }
            total++;
        }
        if(ligne.trimmed().startsWith("Score = ") && longueurOk == true)
        {
            liste2 = ligne.split("=");           // on splite par rapport à "="
            strNombre2 = liste2[2].trimmed();    // on garde juste le troisième élément
            strExpected = strNombre2.split("e-");
            if(liste2[2].contains("e-"))
            {
                int expectedUnites = strExpected[0].toInt();
                int expectedPuissance = strExpected[1].toInt();

                if(expectedPuissance > seuilExpPuiss || (expectedPuissance == seuilExpPuiss && expectedUnites <= seuilExpUnit))
                    nombreOk++;

                longueurOk = false;
            }
            pourcent = QString::number((int)(100*nombreOk / total)) + "% (" + QString::number(nombreOk) + "/" + QString::number(total) + ")";
            if((int)total%1000 == 0)
                emit pourcentageAChange(pourcent, numLigne);
        }
    }
    this->aTermine = true;
    emit pourcentageAChange(pourcent, this->mNumLigne);
}

void threadTest::run()
{
    this->aTermine = false;

    emit this->threadRunning();

    if(this->mSeuilLength == -1)
        testEvalue(this->mNomFichierAParser, this->mSeuilExpUnit, this->mSeuilExpPuiss, this->mNumLigne);

    else if(this->mSeuilExpPuiss == -1 && this->mSeuilExpUnit == -1)
        testLength(this->mNomFichierAParser, this->mSeuilLength, this->mNumLigne);

    else
        testLengthEvalue(this->mNomFichierAParser, this->mSeuilLength, this->mSeuilExpUnit, this->mSeuilExpPuiss, this->mNumLigne);

    emit this->threadTermine();
}
