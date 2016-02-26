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
    widget/dmainwindow.cpp \
    widget/dclosebutton.cpp \
    widget/dpushbutton.cpp \
    widget/dtips.cpp \
    widget/dlineedit.cpp \
    widget/dcombobox.cpp \
    widget/dstepedit.cpp \
    widget/dwidget.cpp \
    widget/dwaterprogress.cpp
HEADERS += \
    widget/dmainwindow.h \
    widget/dconstant.h \
    include/DMainWindow \
    widget/dclosebutton.h \
    include/DCloseButton \
    include/DPushButton \
    widget/dpushbutton.h \
    widget/dtips.h \
    widget/dlineedit.h \
    include/DLineEdit \
    widget/dcombobox.h \
    include/DComboBox \
    widget/dstepedit.h \
    include/DStepEdit \
    widget/dwidget.h \
    include/DTips \
    widget/dwaterprogress.h \
    include/DWaterProgress

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
    dwidget.qrc
