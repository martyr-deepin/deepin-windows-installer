#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T14:56:17
#
#-------------------------------------------------

CONFIG += c++11

QT       += widgets

TARGET = widget
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    widget/stepedit.cpp \
    widget/lineedit.cpp \
    widget/waterprogress.cpp
HEADERS += \
    widget/constant.h \
    widget/stepedit.h \
    widget/lineedit.h \
    widget/waterprogress.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DESTDIR = ./
CONFIG(debug, debug|release) {
    mac: TARGET = $$join(TARGET,,,_debug)
    win32: TARGET = $$join(TARGET,,,d)
}

RESOURCES += \
    widget.qrc
