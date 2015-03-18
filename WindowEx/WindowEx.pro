#-------------------------------------------------
#
# Project created by QtCreator 2014-02-09T17:08:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WindowEx
TEMPLATE = app


SOURCES += main.cpp\
        WindowEx.cpp

HEADERS  += WindowEx.h

FORMS    += WindowEx.ui

LIBS += user32.lib advapi32.lib dwmapi.lib uxtheme.lib
