#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T10:22:04
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TableViewPaging
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    DatabaseManager.cpp \
    TableViewPage.cpp \
    InsertDialog.cpp

HEADERS  += MainWindow.h \
    DatabaseManager.h \
    GlobalConstances.h \
    TableViewPage.h \
    InsertDialog.h

FORMS    += MainWindow.ui \
    InsertDialog.ui
