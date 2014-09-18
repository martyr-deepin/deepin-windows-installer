#pragma once

#include "backend.h"

namespace  DeepinInstaller {

class WindowsBackend: public Backend {
public:
    WindowsBackend(const QString &installTarget,
                   const QString &isoPath,
                   const QString &username,
                   const QString &password);

    virtual int CreateUninstaller();

    virtual bool HasInstalled();

    virtual int  Uninstall();

    virtual int FetchISO();

    virtual int CreateVirtualDisks();

    virtual int CreateConfig();

    virtual int InstallBootloader();

    virtual int InstallGrub();

private:
    bool verfiyMD5(const QString &root, const QString md5file);

    int installBCD();
    int uninstallBCD();

    int installUEFI();
    int uninstallUEFI();
};

}
