#pragma once

#include "backend.h"
#include "progressreporter.h"

namespace  DeepinInstaller
{

class WindowsBackend: public Backend
{
    Q_OBJECT
public:
    enum BootMode{
        SignleBoot,
        MultiBoot,
    };
    Q_ENUM(BootMode)

    WindowsBackend(const QString &username,
                   const QString &hostname,
                   const QString &password,
                   const QString &locale,
                   const QString &installTarget,
                   const QString &isoPath,
                   int installSize,
                   int swapSize,
                   QObject *parent = 0);


    virtual bool CheckReinstall();

    virtual bool HasInstalled();

    static int PreFetchISO(QString &isopath);

    virtual int UninstallApp();

    virtual int UninstallClear();

    virtual int CreateUninstaller();

    virtual int FetchISO();

    virtual int CreateVirtualDisks();

    virtual int CreateConfig();

    virtual int InstallBootloader();

    virtual int InstallGrub();

    virtual int MigrationData();

private:
    bool VerfiyMD5(const QString &root, const QString md5file);

    int InstallMultiBCD(QString &id);
    int UninstallBCD();

    int InstallUEFI(QString &id);
    int UninstallUEFI();

    int InstallMultiBootIni(QString &id);
    int UninstallBootIni();

    int InstallSingleBCD();
    int InstallSingleBootIni(QString &id);

    void BackupBootloader(const QString& profile);
    void RestoreBootloader(const QString& profile);

    bool m_isUninstall;
};


}

enum BootloaderType {
    BCD_UEFI,
    BCD_BIOS,
    BOOT_INI,
    BL_UNKNOW,
};

BootloaderType WindowsBootLoaderType();
