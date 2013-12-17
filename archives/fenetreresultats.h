#ifndef FENETRERESULTATS_H
#define FENETRERESULTATS_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableWidget>

namespace Ui {
class fenetreResultats;
}

class fenetreResultats : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit fenetreResultats(QWidget *parent = 0);
    ~fenetreResultats();

    
private:
    Ui::fenetreResultats *ui;
    QStandardItemModel *modele;
    QTableWidget *table;
    int iter;

public slots:
    void setMessageStatusBar(QString texte);
    void ajouterLigne(QStringList liste);
};

#endif // FENETRERESULTATS_H
