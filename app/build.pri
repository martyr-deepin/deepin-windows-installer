DEFINES += QT_MESSAGELOGCONTEXT

win32{
    isEmpty(LIB_ROOT){
        LIB_ROOT=D:\Develop\Library
    }

    DTK_ROOT = $$LIB_ROOT\deepin-tool-kit
    DTK_BUILD_ROOT = $$DTK_ROOT\build
    DEFINES += STATIC_LIB
    DEFINES += TAGLIB_STATIC

    INCLUDEPATH += $$DTK_ROOT\dwidget
    INCLUDEPATH += $$DTK_ROOT\dwidget\widgets
    INCLUDEPATH += $$DTK_ROOT\dwidget\platforms\windows
    DEPENDPATH += $$DTK_ROOT\dwidget

    INCLUDEPATH += $$DTK_ROOT\dutil
    INCLUDEPATH += $$DTK_ROOT\dutil\dlog
    DEPENDPATH += $$DTK_ROOT\dutil

    INCLUDEPATH += $$DTK_ROOT\dbase
    DEPENDPATH += $$DTK_ROOT\dbase

    CONFIG(release, debug|release) {
        LIBS += -L$$DTK_BUILD_ROOT\dwidget\release -ldtkwidget
        LIBS += -L$$DTK_BUILD_ROOT\dutil\release -ldtkutil
        LIBS += -L$$DTK_BUILD_ROOT\dbase\release -ldtkbase
    } else:CONFIG(debug, debug|release){
        LIBS += -L$$DTK_BUILD_ROOT\dwidget\debug -ldtkwidget
        LIBS += -L$$DTK_BUILD_ROOT\dutil\debug -ldtkutil
        LIBS += -L$$DTK_BUILD_ROOT\dbase\debug -ldtkbase
    }
}
