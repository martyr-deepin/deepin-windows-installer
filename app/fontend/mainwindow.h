#pragma once

#include <DMainWindow>

class DHeaderWidget;

class QLabel;

namespace DeepinInstaller {
    class Backend;
}

class MainWindow : public DeepinWidget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:
    void install();
    void uninstall();

    void maxInstallSizeChage(int);
    void progressChanged(int);

    void showUsernameTips();
    void hideUsernameTips();
    void setUsernameTips(const QString& tips);

    void showPasswordTips();
    void hidePasswordTips();
    void setPasswordTips(const QString& tips);

    void showRepeatPasswordTips();
    void hideRepeatPasswordTips();
    void setRepeatPasswordTips(const QString& tips);

public slots:
    void goInstall();
    void goInstallOptionCheck();
    void goInstallProcess();
    void goInstallSuccess();
    void goInstallFailed();
    void goUninstall();
    void goUninstallPre();
    void goUninstallProcess();
    void goUninstallSuccess();
    void goUninstallFailed();

    void goReInstall();

    void installDone(int);
    void uninstallDone(int);
    void reinstallDone(int);

private slots:
    void installDevTextChanged(const QString&);

    void updateActions(const QString& act);
    void updateProgress(int);

    void reboot();

private:
    void EnableCloseButton(bool);

    QWidget *InstallOptionBody();
    QWidget *InstallFooter();
    QWidget *ExitFooter();

    QWidget *InstallProcessBody();
    QWidget *EmptyFooter();

    QWidget *InstallSuccessBody();
    QWidget *InstallSuccessFooter();

    QWidget *InstallFailedBody();
    QWidget *FinishFooter();

    QWidget *UninstallBody();
    QWidget *UninstallFooter();

    QWidget *UninstallProcessBody();

    QWidget *UninstallSuccessBody();
    QWidget *UninstallFailedBody();

 public slots:
    void setUsername(const QString&);
    void setPassword(const QString&);
    void setRepeatPassword (const QString&);
    void setInstallDev(const QString&);
    void setInstallSize(int);

signals:
    void usernameChanged(const QString&);
    void passwordChanged(const QString&);
    void installDevChanged(const QString&);
    void installSizeChanged(const QString&);

    void progressDone();

    //InstallOption
private:
    QString     m_Username;
    QString     m_Password;
    QString     m_RepeatPassword;
    QString     m_InstallDev;
    int         m_InstallSize;
    bool        m_DiskSizeEnough;

    DeepinInstaller::Backend* m_Backend;
};
