#pragma once

#include <QString>

namespace DeepinInstaller {

struct InstallInfo {
public:
    QString TargetDev;
    QString InstallPath;
    QString ImagePath;
    QString DistroName;
    quint64 RootSize;
    quint64 SwapSize;

    QString KeyboardVariant;
    QString KeyboardLayout;
    QString Locale;
    QString Accessibility;
    QString KernelPath;
    QString InitrdPath;
    QString RootFlags;

    QString Username;
    QString Password;
    QString Hostname;
    QString Timezone;
};

class Backend {
public:
    enum ActionStatus {
        Success,
        Failed,
    };

    Backend(const QString& installTarget,
            const QString& isoPath,
            const QString &username,
            const QString &password);

    virtual int CreateInstallDir();

    virtual int CreateUninstaller() = 0;

    virtual bool HasInstalled() = 0;

    virtual int  Uninstall() = 0;

    virtual int FetchISO() = 0;

    virtual int ExtractISO();

    virtual int CreateVirtualDisks() = 0;

    virtual int CreateConfig() = 0;

    virtual int InstallBootloader() = 0;

    virtual int InstallGrub() = 0;

protected:
    InstallInfo m_Info;
};

}
