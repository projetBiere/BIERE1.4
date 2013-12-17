#ifndef HISTOGRAMME_H
#define HISTOGRAMME_H

#include <QDialog>
#include <QPainter>
#include <QStatusBar>
#include <QVBoxLayout>

namespace Ui {
class Histogramme;
}

class Histogramme : public QDialog
{
    Q_OBJECT
    
public:
    explicit Histogramme(QWidget *parent = 0);
    Histogramme(QString nomFichier, QVector<int> tabValeurs);
    ~Histogramme();
    QString getNomFichier();
    
private:
    Ui::Histogramme *ui;
    QString nomFichier;
    float tabValeurs[100];
    int maxTab;
    QRectF tabRectangles[100];
    int pointDeDepart;
    int dernierElement;

protected:
    void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *event);
};

#endif // HISTOGRAMME_H
