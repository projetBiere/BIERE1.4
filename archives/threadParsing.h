#ifndef THREADPARSING_H
#define THREADPARSING_H

#include "fenetreresultats.h"
#include <QThread>
#include <QtCore>
#include <QStandardItemModel>
#include <QTableWidgetItem>

class threadParsing : public QThread
{
    Q_OBJECT
public:
    explicit threadParsing(QObject *parent = 0);
    threadParsing(QString nomFichierAParser, int seuilLength, int seuilExpUnit, int seuilExpPuiss, int numLigne, fenetreResultats *fenetre);
    void run();

private:
    QString mNomFichierAParser;
    int mSeuilLength;
    int mSeuilExpUnit;
    int mSeuilExpPuiss;
    int mNumLigne;
    fenetreResultats *mFenetre;
    bool mATermine;
    QStandardItemModel *modele;
    int mElementsAjoutes;
    float mGlobalseqdb;

signals:
    void threadRunning();
    void changerTexteStatusBar(QString texte);
    void ajouterLigneAuModele(QStringList liste);
    void threadTermine();
    
public slots:
    
};

#endif // THREADPARSING_H
