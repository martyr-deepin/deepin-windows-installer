#include <QApplication>

#include "fontend/mainwindow.h"

#include "backend/backendfactory.h"
#include "backend/winbackend.h"

#include "config/log.h"
#include "config/config.h"

#include <xsys.h>


#include <QApplication>
#include <QStandardPaths>
#include <QProcess>
#include <Windows.h>

using namespace DeepinInstaller;

void GenerateFontImage();

int main(int argc, char**argv) {
    Q_INIT_RESOURCE(xsys);
    Q_INIT_RESOURCE(dwidget);
    QApplication app(argc, argv);
    InstallLogHandler();
    LoadFonts();

//    if (WindowsBackend::HasInstalled ()
//            &&
//    && !QApplication::arguments().contains ("-uninstall")) {
//        QString uninstallerPath = Xapi::InsertTmpFile (QApplication::applicationFilePath ());
//        qDebug()<<"Create Uninstaller"<<uninstallerPath;
//        QProcess uninstller;
//        uninstller.startDetached (QString("\"%1\"").arg (uninstallerPath) + " -uninstall");
//        return 0;
//    }

    MainWindow w;
    w.setWindowTitle (QObject::tr("Deepin Windows Installer"));
    w.show();

    int ret = app.exec();
    qDebug()<<"app.exec() return with"<<ret;
    return 0;
}
