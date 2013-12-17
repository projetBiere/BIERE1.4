#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "threadTest.h"
#include "fenetreresultats.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QVector>

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


private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QStandardItem *item;
    QStandardItem *nbre;
    threadTest *mThread;

public slots:
    void changerTexteStatusBar(QString newMessage);
    void changerTextePourcentage(QString newMessage);
    void changerPourcentageModele(QString newPourcentage, int numLigne);
    void tailleAutoDesColonnes(QString rien, int zero);
    void voirSiTousLesThreadsOntTermine();
    void lesThreadOntTermine();

private slots:
    void on_boutonTestLength_clicked();
    void on_boutonOk_clicked();
    void on_boutonAjouterFichiers_clicked();

    void on_boutonEnleverFichiers_clicked();

signals:
    void clicSurStop();
    void threadsTermines();
};

#endif // MAINWINDOW_H
