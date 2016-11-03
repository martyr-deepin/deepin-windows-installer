#-------------------------------------------------
#
# Project created by QtCreator 2014-09-30T10:21:17
#
#-------------------------------------------------

QT       -= gui

TARGET = libuefi
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    src/Firmware.cpp \
    src/Utils.cpp \
    src/BootOrder.cpp \
    src/GUID.cpp \
    src/BootOption.cpp

HEADERS += \
    src/Firmware.h \
    src/BootOrder.h \
    src/Type.h \
    src/GUID.h \
    include/Firmware \
    src/BootOption.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DESTDIR = ./
CONFIG(debug, debug|release) {
    mac: TARGET = $$join(TARGET,,,_debug)
    win32: TARGET = $$join(TARGET,,,d)
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../xsys/ -lxsys
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../xsys/ -lxsysd
else:unix: LIBS += -L$$OUT_PWD/../xsys/ -lxsys

INCLUDEPATH += $$PWD/../xsys
DEPENDPATH += $$PWD/../xsys

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../xsys/libxsys.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../xsys/libxsysd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../xsys/xsys.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../xsys/xsysd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../xsys/libxsys.a

OTHER_FILES += \
    README.md
