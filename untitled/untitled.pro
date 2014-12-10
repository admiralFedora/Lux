#-------------------------------------------------
#
# Project created by QtCreator 2014-11-27T19:09:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    adjusters.cpp \
    motor.cpp \
    controls.cpp \
    meterWidget.cpp

HEADERS  += widget.h \
    adjusters.h \
    motor.h \
    controls.h \
    meterWidget.h

FORMS    += widget.ui

RESOURCES += \
    Images.qrc
