#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "threadTest.h"
#include "histogramme.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QVector>
#include <QSettings>
#include <QMessageBox>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void extraireEchantillons(QString nomFichier, int numLigne, QVector<int> seuils); // [0]=length [1]=evalueUnit [2]=evalueExp [3]=score
    void histoEvalue(int numLigne);

protected:
    void closeEvent(QEvent *event);

private:
    QDate dateToday;
    QTime timeToday;
    QString strTime;
    QString strDate;
    QString outputDate;
    QString cheminOutput;
    QString cheminTemp;
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QStandardItem *item;
    QStandardItem *nbre;
    threadTest *mThread;
    int optLength;
    int optEvalueUnit;
    int optEvalueExp;
    QString optDossierDeTravail;
    QVector< QVector<int> > tabValeurs;

public slots:
    void changerTexteStatusBar(QString newMessage);
    void changerPourcentageModele(QString newPourcentage, int numLigne);
    void voirSiTousLesThreadsOntTermine();
    void voirSiTousLesParsingOntTermine();
    void lesThreadOntTermine();
    void lesParsingOntTermine();
    void changerTexteFichier(int numLigne, QString newTexte);
    void afficherHistogramme(int numero);

private slots:
    void on_boutonTestLength_clicked();
    void on_boutonParse_clicked();
    void on_boutonAjouterFichiers_clicked();
    void on_boutonEnleverFichiers_clicked();
    void on_boutonQuitter_clicked();
    void on_actionA_propos_triggered();

    void on_boutonHisto_clicked();

signals:
    void texteStatusBarChange(QString newMessage);
    void clicSurStop();
    void threadsTermines();
    void finiDeCompterLesLignes();
    void texteFichierAChange(int numLigne, QString newTexte);
    void parsingTermine();
    void parsingTermines();
    void parsingHistoTermine(int numero);
};

#endif // MAINWINDOW_H
