#-------------------------------------------------
#
#
#-------------------------------------------------

system($$PWD\tools\importPo.bat $$PWD\app\app.pro)

TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS  += \
    lib/xsys \
    lib/libuefi \
    lib/dwidget \
    app \
