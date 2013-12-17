#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("M:/qt/BIERE/BIERE") + locale);
    //translator.load("M:/qt/BIERE/BIERE_en");

    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
