#-------------------------------------------------
#
# Project created by QtCreator 2019-01-31T12:38:30
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookManager
TEMPLATE = app


SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS  += \
    connection.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

CONFIG += c++11
