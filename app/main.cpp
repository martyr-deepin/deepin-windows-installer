#include <QApplication>

#include "fontend/mainwindow.h"

#include "backend/backendfactory.h"
#include "backend/winbackend.h"

#include "config/log.h"

using namespace DeepinInstaller;

int main(int argc, char**argv) {
    QApplication app(argc, argv);

    InstallLogHandler();

    MainWindow w;
    w.show();
    WindowsBackend* backend = dynamic_cast<WindowsBackend*>(
                BackendFactory::Instance().CreateBackend(
                    BackendFactory::Windows, "C:/", "C:/deepin.iso"));
    if (backend->HasInstalled()) {
        backend->Uninstall();
        return 0;
    }

//    backend->CreateInstallDir();
//    backend->CreateUninstaller();
//    backend->FetchISO();
//    backend->ExtractISO();
//    backend->CreateVirtualDisks();
//    backend->CreateConfig();
//   backend->InstallBootloader();
 //   backend->InstallGrub();
    return 0;
    return app.exec();
}
