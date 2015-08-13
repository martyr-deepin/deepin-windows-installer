#pragma once

#include <xutil.h>

#include <DMainWindow>

class DHeaderWidget;
class DLineEdit;
class QLabel;

namespace DeepinInstaller {
    class Backend;
}

class MainWindow : public DeepinWidget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

    virtual void keyReleaseEvent(QKeyEvent *);

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
    void installLanguageChanged(int);
    void installDevTextChanged(const QString&);

    void updateActions(const QString& act);
    void updateProgress(int);

    void reboot();

private:
    void EnableCloseButton(bool);

    DHeaderWidget* Header();

    QWidget *InstallOptionBody();
    QWidget *InstallFooter();
    QWidget *ExitFooter();
    QWidget *NoSpaceBody(const QString& hits);

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
    void editUsernameBegin(const QString&);
    void editUsernameFinish();
    void setPassword(const QString&);
    void editPasswordBegin(const QString&);
    void editPasswordFinish();
    void setRepeatPassword (const QString&);
    void editRepeatPasswordBegin(const QString&);
    void editRepeatPasswordFinish();
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
    QString     m_InstallLocale;
    QString     m_InstallDev;
    int         m_InstallSize;
    bool        m_DiskSizeEnough;

    bool        m_EnableClose;

    DeepinInstaller::Backend* m_Backend;

    QVector<XUtils::Language> m_Languages;
};
