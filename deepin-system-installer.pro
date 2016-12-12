#-------------------------------------------------
#
#
#-------------------------------------------------

DEFINES += QT_MESSAGELOGCONTEXT

win32 {
    system($$PWD/tools/build-version.bat $$PWD/app/data/build_env.h)
    system($$PWD/tools/importPo.bat $$PWD/app/app.pro)
}

TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS  += \
    lib/xsys \
    lib/libuefi \
    lib/widget \
    app \
