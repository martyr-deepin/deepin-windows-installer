#-------------------------------------------------
#
#
#-------------------------------------------------

CONFIG += c++11

linux{
    QMAKE_EXT_OBJ = .bc
    QMAKE_CXXFLAGS += -emit-llvm
    QMAKE_CXX = clang++
    QMAKE_CC = clang
    QMAKE_LIB = llvm-ld -link-as-library -o
}

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-installer
TEMPLATE = app

SOURCES += \
    backend/backend.cpp \
    main.cpp \
    fontend/mainwindow.cpp \
    config/config.cpp \
    backend/backendfactory.cpp \
    config/log.cpp \
    backend/winuefi.cpp

HEADERS += \ 
    backend/backend.h \
    fontend/mainwindow.h \
    backend/backendfactory.h \
    config/log.h

win32 {
SOURCES += \
    backend/winbackend.cpp

HEADERS += \
    backend/winbackend.h
}

RESOURCES += \
     bootloader.qrc \
    blobs.qrc

DESTDIR = ./

win32-msvc* {
    DEFINES += _USING_V110_SDK71_
    QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"
    QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS",5.1"
}

#-------------------------------------------------
# libxsys
#-------------------------------------------------
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/xsys/ -lxsys
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/xsys/ -lxsysd
else:unix: LIBS += -L$$OUT_PWD/../lib/xsys/ -lxsys
d
INCLUDEPATH += $$PWD/../lib/xsys
DEPENDPATH += $$PWD/../lib/xsys

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/xsys/libxsys.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/xsys/libxsysd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/xsys/xsys.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/xsys/xsysd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/xsys/libxsys.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/dwidget/ -ldeepin-widget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/dwidget/ -ldeepin-widgetd
else:unix: LIBS += -L$$OUT_PWD/../lib/dwidget/ -ldeepin-widget

INCLUDEPATH += $$PWD/../lib/dwidget/include/
DEPENDPATH += $$PWD/../lib/dwidget

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/dwidget/libdeepin-widget.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/dwidget/libdeepin-widgetd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/dwidget/deepin-widget.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/dwidget/deepin-widgetd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/dwidget/libdeepin-widget.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/libuefi/ -llibuefi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/libuefi/ -llibuefid
else:unix: LIBS += -L$$OUT_PWD/../lib/libuefi/ -llibuefi

INCLUDEPATH += $$PWD/../lib/libuefi
DEPENDPATH += $$PWD/../lib/libuefi

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/libuefi/liblibuefi.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/libuefi/liblibuefid.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/libuefi/libuefi.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/libuefi/libuefid.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/libuefi/liblibuefi.a
