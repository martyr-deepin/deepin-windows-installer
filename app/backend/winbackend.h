#pragma once

#include "backend.h"
#include "progressreporter.h"

namespace  DeepinInstaller {

class WindowsBackend: public Backend {
public:
    WindowsBackend(const QString &username,
                   const QString &password,
                   const QString &locale,
                   const QString &installTarget,
                   const QString &isoPath,
                   int installSize,
                   QObject *parent = 0);

    virtual bool HasInstalled();

    static int PreFetchISO(QString &isopath);

    virtual int UninstallApp();

    virtual int CreateUninstaller();

    virtual int FetchISO();

    virtual int CreateVirtualDisks();

    virtual int CreateConfig();

    virtual int InstallBootloader();

    virtual int InstallGrub();

private:
    bool VerfiyMD5(const QString &root, const QString md5file);

    int InstallBCD(QString &id);
    int UninstallBCD();

    int InstallUEFI(QString &id);
    int UninstallUEFI();

    int InstallBootIni(QString &id);
    int UninstallBootIni();
};


}

enum BootloaderType{
    BCD_UEFI,
    BCD_BIOS,
    BOOT_INI,
    BL_UNKNOW,
};

BootloaderType WindowsBootLoaderType();

