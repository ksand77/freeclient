#-------------------------------------------------
#
# Project created by QtCreator 2015-10-31T21:14:50
#
#-------------------------------------------------

QT       += core gui webkit webkitwidgets network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clientfreefilm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

RC_ICONS = movie.ico
