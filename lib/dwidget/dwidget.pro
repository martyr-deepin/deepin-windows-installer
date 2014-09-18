#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T14:56:17
#
#-------------------------------------------------

CONFIG += c++11

QT       += widgets

TARGET = deepin-widget
TEMPLATE = lib
CONFIG += staticlib


SOURCES += \
    widget/dmainwindow.cpp
HEADERS += \
    widget/dmainwindow.h \
    widget/dconstant.h \
    include/DMainWindow

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DESTDIR = ./
CONFIG(debug, debug|release) {
    mac: TARGET = $$join(TARGET,,,_debug)
    win32: TARGET = $$join(TARGET,,,d)
}

win32-msvc* {
    DEFINES += _USING_V110_SDK71_
    QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"
    QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS",5.1"
}
