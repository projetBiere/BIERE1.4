#ifndef THREADTEST_H
#define THREADTEST_H

#include <QThread>
#include <QtCore>

class threadTest : public QThread
{
    Q_OBJECT
public:
    explicit threadTest(QObject *parent = 0);
    threadTest(QString nomFichierAParser, int seuilLength, int seuilExpUnit, int seuilExpPuiss, int numLigne);
    void testLength(QString nomFichierAParser, int seuilLength, int numLigne);
    void testEvalue(QString nomFichierAParser, int seuilExpUnit, int seuilExpPuiss, int numLigne);
    void testLengthEvalue(QString nomFichierAParser, int seuilLength, int seuilExpUnit, int seuilExpPuiss, int numLigne);
    void run();
    bool aTermine;

protected:


private:
    QString mNomFichierAParser;
    int mSeuilLength;
    int mSeuilExpUnit;
    int mSeuilExpPuiss;
    int mNumLigne;
    QString mMessageDErreur;
    
signals:
    void messageDErreurAChange(QString newMessage);
    void pourcentageAChange(QString newPourcentage, int numLigne);
    void threadRunning();
    void threadTermine();
    
public slots:
    void stopperThread();
    
};

#endif // THREADTEST_H
