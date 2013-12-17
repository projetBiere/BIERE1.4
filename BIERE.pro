#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T09:56:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BIERE
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    threadTest.cpp \
    histogramme.cpp

HEADERS  += mainwindow.h \
    threadTest.h \
    histogramme.h

FORMS    += mainwindow.ui \
    histogramme.ui
