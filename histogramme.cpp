#include "histogramme.h"
#include "ui_histogramme.h"
#include <QDebug>
#include <QMouseEvent>
#include <QToolTip>

Histogramme::Histogramme(QWidget *parent) : QDialog(parent), ui(new Ui::Histogramme)
{
    ui->setupUi(this);

    for(int i = 0; i<100; i++)
    {
        tabRectangles[i].setTopLeft(QPoint(25 + i*6, 300-25));
        int alea = qrand()%265;
        tabRectangles[i].setSize(QSize(5,-alea));
    }
}

Histogramme::Histogramme(QString nomFichier, QVector<int> tabValeurs)
{
    ui->setupUi(this);

    this->nomFichier = nomFichier;
    this->setWindowTitle(nomFichier);
    maxTab = 0;
    dernierElement = 0;

    for(int i = 0; i<100; i++)
    {
        if(tabValeurs[i] > maxTab)
            maxTab = tabValeurs[i];

        if(tabValeurs[i] != 0)
            dernierElement = i;
    }

    for(int i = 0; i<100; i++)
    {
        pointDeDepart = (640 - dernierElement*6)/2;
        tabRectangles[i].setTopLeft(QPoint(pointDeDepart + i*6, 300-25));
        if(maxTab>250)
            tabRectangles[i].setSize(QSize(5,-tabValeurs[i]*250/maxTab));
        else
            tabRectangles[i].setSize(QSize(5,-tabValeurs[i]));

    }
}

QString Histogramme::getNomFichier()
{
    return this->nomFichier;
}

Histogramme::~Histogramme()
{
    delete ui;
    qDebug() << "Destructeur de " << this->nomFichier;
}

void Histogramme::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);

    for(int i = 0; i<100; i++)
    {
        painter.drawRect(tabRectangles[i]);
        painter.fillRect(tabRectangles[i], QColor(0,102,102));
    }

    QPainter painter2(this);
    painter2.setPen(Qt::black);

    // axe x + sa légende
    painter2.drawLine(pointDeDepart - 10, 300-25, pointDeDepart + dernierElement*6 + 15, 300-25);
    painter2.drawText(pointDeDepart + dernierElement*6 + 25, 300-12,"e-value (10-)");

    // axe y + sa légende
    painter2.drawLine(pointDeDepart, 300-25+10, pointDeDepart, 25);
    painter2.drawText(pointDeDepart - 70, 50,"Nombre");
    painter2.drawText(pointDeDepart - 70, 65,"d'occurences");
}

void Histogramme::mouseMoveEvent(QMouseEvent *event)
{
    int n = (event->x() - pointDeDepart)/6;
    QPainter painter2(this);
    painter2.setPen(Qt::black);
    if(n>=0 && n<=99)
    {
        if(maxTab>250)
        {
            if(event->y() >= 300-25+tabRectangles[n].height() && event->y() <= 300-25)
                /*this->setWindowTitle(nomFichier + " -- Nombre d'occurrences pour 10(-" + QString::number(n)
                         + ") : "
                         + QString::number((int)(-tabRectangles[n].height()*maxTab/250)));*/
                this->setToolTip("Nombre d'occurences pour 10<sup>-" + QString::number(n)+ "</sup> : " + QString::number((int)-tabRectangles[n].height()*maxTab/250));

            else
                this->setWindowTitle(nomFichier);
        }
        else
        {
            if(event->y() >= 300-25+tabRectangles[n].height() && event->y() <= 300-25)
                /*this->setWindowTitle(nomFichier + " -- Nombre d'occurrences pour 10(-" + QString::number(n)
                         + ") : "
                         + QString::number(-tabRectangles[n].height()));*/
                this->setToolTip("Nombre d'occurences pour 10<sup>-" + QString::number(n)+ "</sup> : " + QString::number(-tabRectangles[n].height()));

            else
                this->setWindowTitle(nomFichier);
        }
    }
}
