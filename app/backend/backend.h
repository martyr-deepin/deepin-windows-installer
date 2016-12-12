#pragma once

#include <QString>
#include <QObject>
#include <QDebug>

class QNetworkReply;

namespace DeepinInstaller {

struct InstallInfo {
public:
    QString TargetDev;
    QString InstallPrefix;
    QString InstallPath;
    QString ImagePath;
    QString DistroName;
    quint64 RootSize;
    quint64 SwapSize;
    QString RootFilePath;
    QString SwapFilePath;

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
    QString Version;
    QString ReleaseInfo;

    QString BootMethod;
};

class Backend:public QObject {
    Q_OBJECT
public:
    enum ActionStatus {
        Success,
        Failed,
    };
    explicit Backend(
            const QString &username,
            const QString &hostname,
            const QString &password,
            const QString &locale,
            const QString& installTarget,
            const QString& isoPath,
            int installSize, int swapSize,
            QObject *parent = 0);

    void SetInstallParam(const QString &username,
                         const QString &hostname,
                         const QString &password,
                         const QString &locale,
                         const QString& installTarget,
                         const QString& isoPath,
                         int installSize, int swapSize);

    int Go();

    int GoBack();

    int Progress() const;

    const QString& Action() const;

    const QString& Error() const;

    virtual bool CheckReinstall() = 0;

    virtual bool HasInstalled() = 0;

    //5
    virtual int CreateInstallDir();

    //5
    virtual int CreateUninstaller() = 0;

    //50
    virtual int FetchISO() = 0;

    //10 add extract window hooks
    virtual int ExtractISO();

    //5
    virtual int CreateVirtualDisks() = 0;

    //1
    virtual int CreateConfig() = 0;

    //3
    virtual int InstallBootloader() = 0;

    //1
    virtual int InstallGrub() = 0;

    // 20
    virtual int MigrationData() = 0;

    virtual int  UninstallApp() = 0;

    virtual int  UninstallClear() = 0;

    QString Release();

    void Report(quint64 total, quint64 copyed);

signals:
    void Done(int);
    void Install();
    void Uninstall();

    void ActionUpdate(const QString& act);
    void ProgressUpdate(int progress);

public slots:
    void AsyncInstall();
    void AsyncUninstall();

    void DownloadMD5ListFinish();

protected:
    void Init(const QString &username,
              const QString &hostname,
              const QString &password,
              const QString &locale,
              const QString& installTarget,
              const QString& isoPath,
              int installSize,
              int swapSize);

    void FetchMD5List();

    QMap<QString, QString> m_MD5Map;
    QNetworkReply *m_Reply;
    bool m_DownloadFinsh;

    QString FetchImageFiles();
    QString GetRelesaseVersion(const QString& imagePath);

    void SetError(const QString& err) {m_LastError = err;}

    void SetAction(const QString& act) {
        m_CurAction = act;
        emit ActionUpdate(act);
    }

    void Increment(int deta) {
        qDebug()<<"Increment"<<deta<<m_Progress;
        m_Progress += deta;
        emit ProgressUpdate (m_Progress);
    }

    InstallInfo m_Info;
    QString     m_LastError;
    QString     m_CurAction;

    int         m_BasePersent;
    int         m_BaseRange;
    int         m_Progress;
};


typedef int (Backend::*InstallAction)(void);

const QString Version = "v1.0";

const QString AppTitle();

}
