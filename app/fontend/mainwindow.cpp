#include "mainwindow.h"

#include "widgets/dheaderwidget.h"
#include "widgets/dfooterwidget.h"

#include "../backend/winbackend.h"
#include "../backend/backendfactory.h"
#include "../backend/utils.h"

#include "../config/config.h"

#include <xsys.h>
#include <xutil.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>

#include <DWaterProgress>
#include <DPushButton>
#include <DLineEdit>
#include <DComboBox>
#include <DStepEdit>
#include <DTips>

#include <QApplication>
#include <QStandardPaths>
#include <QProcess>
#include <QPainter>
#include <QKeyEvent>

static const int DefaultWidgetHeight = 24;

static const int DefaultMaxInstallSize = 64;

static const QString FailIconURL = ":/fontend/images/fail.png";
static const QString SuccessIconURL = ":/fontend/images/success.png";
static const QString WarningIconURL = ":/fontend/images/warning.png";
static QString PasswordHits;
static QString RepeatPasswordHits;

using namespace DeepinInstaller;

MainWindow::MainWindow(QWidget *parent) :
    DeepinWidget::DMainWindow(parent)
{
    m_AcceptKey = false;
    PasswordHits = QObject::tr("Password can not be empty.");
    RepeatPasswordHits = QObject::tr("The two passwords do not match.");

    setFixedSize(340, 520);

    connect(this, SIGNAL(install()), this, SLOT(goInstall()));
    connect(this, SIGNAL(uninstall()), this, SLOT(goUninstall()));

    m_Backend = BackendFactory::Instance().CreateBackend(BackendFactory::Windows);
    m_Languages = XUtils::LoadSupportLanguage();
    if (m_Backend->HasInstalled()) {
        emit uninstall();
    } else {
        emit install();
    }
//    goInstallSuccess();
}

void MainWindow::keyReleaseEvent(QKeyEvent *k)
{
    if (m_AcceptKey && (k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter)) {
        this->goInstallOptionCheck();
    }
}

QWidget *MainWindow::InstallOptionBody()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->addSpacing(10);

    DLineEdit *usernameEdit = new DLineEdit(":/fontend/images/user.png", tr("Username"));
    usernameEdit->setMaxLength(32);
    usernameEdit->setFixedSize(180, DefaultWidgetHeight);
    layout->addWidget(usernameEdit);
    layout->setAlignment(usernameEdit, Qt::AlignCenter);
    connect(usernameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(setUsername(QString)));
    connect(usernameEdit, SIGNAL(editingFinished()),
            this, SLOT(editUsernameFinish()));
    connect(usernameEdit, SIGNAL(editingBegin(QString)),
            this, SLOT(editUsernameBegin(QString)));

    connect(this, SIGNAL(usernameChanged(QString)),
            usernameEdit, SLOT(overwriteText(QString)));


    DTips *usernameTips = new DTips(usernameEdit);
    connect(this, SIGNAL(showUsernameTips()),
            usernameTips, SLOT(pop()));
    connect(this, SIGNAL(hideUsernameTips()),
            usernameTips, SLOT(pack()));
    connect(this, SIGNAL(setUsernameTips(QString)),
            usernameTips, SLOT(setText(QString)));

    usernameEdit->setText(ToDeepinUsername(Xapi::Username()));

    DLineEdit *password = new DLineEdit(":/fontend/images/lock.png", tr("Password"));
    password->setEchoMode(QLineEdit::Password);
    password->setFixedSize(180, DefaultWidgetHeight);
    layout->addSpacing(27);
    layout->addWidget(password);
    layout->setAlignment(password, Qt::AlignCenter);
    connect(password, SIGNAL(textChanged(QString)),
            this, SLOT(setPassword(QString)));
    connect(password, SIGNAL(editingFinished()),
            this, SLOT(editPasswordFinish()));
    connect(password, SIGNAL(editingBegin(QString)),
            this, SLOT(editPasswordBegin(QString)));

    DTips *passwordTips = new DTips(password);
    passwordTips->setText(PasswordHits);
    connect(this, SIGNAL(showPasswordTips()),
            passwordTips, SLOT(pop()));
    connect(this, SIGNAL(hidePasswordTips()),
            passwordTips, SLOT(pack()));
    connect(this, SIGNAL(setPasswordTips(QString)),
            passwordTips, SLOT(setText(QString)));
    setPassword("");

    DLineEdit *repeatRassword = new DLineEdit(":/fontend/images/lock.png", tr("Repeat Password"));
    repeatRassword->setEchoMode(QLineEdit::Password);
    repeatRassword->setFixedSize(180, DefaultWidgetHeight);
    layout->addSpacing(27);
    layout->addWidget(repeatRassword);
    layout->setAlignment(repeatRassword, Qt::AlignCenter);
    connect(repeatRassword, SIGNAL(textChanged(QString)),
            this, SLOT(setRepeatPassword(QString)));
    connect(repeatRassword, SIGNAL(editingFinished()),
            this, SLOT(editRepeatPasswordFinish()));
    connect(repeatRassword, SIGNAL(editingBegin(QString)),
            this, SLOT(editRepeatPasswordBegin(QString)));

    DTips *repeatPasswordTips = new DTips(repeatRassword);
    repeatPasswordTips->setText(RepeatPasswordHits);
    connect(this, SIGNAL(showRepeatPasswordTips()),
            repeatPasswordTips, SLOT(pop()));
    connect(this, SIGNAL(hideRepeatPasswordTips()),
            repeatPasswordTips, SLOT(pack()));
    connect(this, SIGNAL(setRepeatPasswordTips(QString)),
            repeatPasswordTips, SLOT(setText(QString)));

    setRepeatPassword("");

    QString defaultLocale = Xapi::Locale();
    DComboBox *installLang = new DComboBox;
    installLang->setFixedSize(180, DefaultWidgetHeight);
    layout->addSpacing(27);
    layout->addWidget(installLang);
    layout->setAlignment(installLang, Qt::AlignCenter);
    installLang->setIconSize(QSize(130, 14));
    installLang->setContentsMargins(QMargins(0, 0, 0, 0));
    installLang->setMinimumContentsLength(0);
    QVector<XUtils::Language>::Iterator langItor = m_Languages.begin();
    int langIndex = 0;
    int defalutLangIndex = 0;
    for (; langItor != m_Languages.end(); ++langItor) {
        installLang->addItem(QIcon(":/data/langicon/" + langItor->Locale + ".png"), "");
        if (defaultLocale.toLower() == langItor->Locale.toLower()) {
            defalutLangIndex = langIndex;
        }
        ++langIndex;
    }

    connect(installLang, SIGNAL(currentIndexChanged(int)),
            this, SLOT(installLanguageChanged(int)));

    installLang->setCurrentIndex(defalutLangIndex);

    DComboBox *installDev = new DComboBox;
    installDev->setFixedSize(180, DefaultWidgetHeight);
    layout->addSpacing(27);
    layout->addWidget(installDev);
    layout->setAlignment(installDev, Qt::AlignCenter);
    connect(installDev, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(installDevTextChanged(QString)));

    // Begin dump disk info

    QString partitionTableFiter = "GPT,MBR";
    BootloaderType bootloader = WindowsBootLoaderType();
    PartitonStyle styleFiter = MBRPartition;
    QString style = "MBR";
    qDebug() << "bootload" << bootloader;
    if (BCD_UEFI == bootloader) {
        styleFiter = GPTPartition;
        style = "GPT";
    }

    bool hasMismatchPartitionStyle = false;
    //Init Dev List
    m_DiskSizeEnough = false;
    QList<DiskInfo> alldisklist = GetLocalDiskList(0, "NTFS", partitionTableFiter);
    QList<DiskInfo> list;
    QList<DiskInfo>::iterator itor;
    for (itor = alldisklist.begin(); itor != alldisklist.end(); ++itor) {
        if (styleFiter == itor->Style) {
            qDebug() << "get disk" << itor->Name << itor->FreeSpace << itor->Encrypt;
            if (itor->FreeSpace > MiniInstallSize && ! itor->Encrypt) {
                qDebug() << "Add  Disk" << itor->Name;
                list.push_back(*itor);
            }
        } else {
            qDebug() << "hasMismatchPartitionStyle disk:" << itor->Name << itor->FreeSpace << itor->Encrypt;
            hasMismatchPartitionStyle = true;
        }
    }

    for (itor = list.begin(); itor != list.end(); ++itor) {
        if (itor->FreeSpace > MiniInstallSize) {
            m_DiskSizeEnough = true;
        }
        installDev->addItem(QString("%1(%2GB)").arg(itor->Name).arg(itor->FreeSpace));
    }

    DStepEdit *installSize = new DStepEdit;
    installSize->setMin(MiniInstallSize);
    if (!list.isEmpty()) {
        installSize->setMax(list.first().FreeSpace);
    } else {
        installSize->setMax(MiniInstallSize + 1);
    }
    installSize->setFixedSize(180, DefaultWidgetHeight);
    layout->addSpacing(24);
    layout->addWidget(installSize);
    layout->setAlignment(installSize, Qt::AlignCenter);
    connect(this, SIGNAL(maxInstallSizeChage(int)),
            installSize, SLOT(setMax(int)));
    connect(installSize, SIGNAL(valueChanged(int)),
            this, SLOT(setInstallSize(int)));
    layout->addSpacing(5);

    installSize->setValue(MiniInstallSize);

    QLabel *hits = new QLabel;
    hits->setFixedWidth(240);
    hits->setWordWrap(true);
    hits->setText("<p style='color:grey; font-size:10px;'>" +
                  tr("This operation will not affect any of your data. Please use it freely.") +
                  "</p>");
    layout->addSpacing(10);
    layout->addWidget(hits);
    layout->setAlignment(hits, Qt::AlignCenter);

    //Disable if there is not enough disk size
    if (!m_DiskSizeEnough) {
        qDebug() << "Disable all control";
        usernameEdit->setVisible(false);
        password->setVisible(false);
        repeatRassword->setVisible(false);
        usernameTips->pack();
        passwordTips->pack();
        installLang->setVisible(false);
        installDev->setVisible(false);
        installSize->setVisible(false);
        if (!hasMismatchPartitionStyle) {
            style = "";
        } else {
            style = " " + style;
        }

        QString errHits = tr("Please ensure that there is at least one%1 disk"
                             " having more than %2GB free space.").arg(style).arg(MiniInstallSize);

        return NoSpaceBody(errHits);
    }
    widget->setLayout(layout);
    return widget;
}

QWidget *MainWindow::InstallFooter()
{
    DPushButton *start = new DPushButton(tr("Start"));
    connect(start, SIGNAL(clicked()), this, SLOT(goInstallOptionCheck()));
    QList<DPushButton *> btlist;
    btlist.append(start);
    return new DFooterWidget(btlist);
}

QWidget *MainWindow::ExitFooter()
{
    DPushButton *start = new DPushButton(tr("Exit"));
    connect(start, SIGNAL(clicked()), this, SLOT(close()));
    QList<DPushButton *> btlist;
    btlist.append(start);
    return new DFooterWidget(btlist);
}

QWidget *HitsBodyWidget(const QString &icon, const QString &text)
{
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(20);
    layout->addSpacing(70);

    QLabel *iconHits = new QLabel;
    iconHits->setPixmap(QPixmap(icon));
    layout->addWidget(iconHits);
    layout->setAlignment(iconHits, Qt::AlignCenter);

    QLabel *textHits = new QLabel;
    textHits->setAlignment(Qt::AlignCenter);
    textHits->setFixedWidth(260);
    textHits->setWordWrap(true);
    textHits->setText(text);
    layout->addWidget(textHits);
    layout->setAlignment(textHits, Qt::AlignCenter);

    widget->setLayout(layout);
    return widget;
}

QWidget *MainWindow::NoSpaceBody(const QString &errhits)
{
    QString hits = "<p style='color:white; font-size:14px;'>" + errhits + "</p>";
    return HitsBodyWidget(WarningIconURL, hits);
}

QWidget *MainWindow::InstallProcessBody()
{
    QString hits = "<p style='color:white; font-size:14px;'>" +
                   tr("Being installed. Please wait... ") +
                   "</p>";
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(20);
    layout->addSpacing(70);

    DWaterProgress *iconHits = new DWaterProgress;
    layout->addWidget(iconHits);
    layout->setAlignment(iconHits, Qt::AlignCenter);
    connect(this, SIGNAL(progressChanged(int)),
            iconHits, SLOT(setProgress(int)));

    connect(this, SIGNAL(progressDone()),
            iconHits, SLOT(stop()));

    iconHits->start();

    QLabel *textHits = new QLabel;
    textHits->setAlignment(Qt::AlignCenter);
    textHits->setFixedWidth(260);
    textHits->setWordWrap(true);
    textHits->setText(hits);

    layout->addWidget(textHits);
    layout->setAlignment(textHits, Qt::AlignCenter);

    widget->setLayout(layout);
    return widget;
}

QWidget *MainWindow::EmptyFooter()
{
    QList<DPushButton *> btlist;
    return new DFooterWidget(btlist);
}

QWidget *MainWindow::InstallSuccessBody()
{
    QString hitsTemplate =
        "<p style='color:white; font-size:12px;'>%1</p>"
        "<p style='color:white; font-size:12px;'>%2</p>";
    QString hits = hitsTemplate.arg(tr("Fast Startup in Windows should be disabled to continue the installation")).arg(tr("Please follow the steps:"));

    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(5);
    layout->addSpacing(5);

    QLabel *iconHits = new QLabel;
    iconHits->setPixmap(QPixmap(":/fontend/images/dialog_warning.png"));
    layout->addWidget(iconHits);
    layout->setAlignment(iconHits, Qt::AlignCenter);

    layout->addStretch();

    QLabel *textHits = new QLabel;
    textHits->setAlignment(Qt::AlignCenter);
    textHits->setFixedWidth(280);
    textHits->setWordWrap(true);
    textHits->setText(hits);
    layout->addWidget(textHits);
    layout->setAlignment(textHits,
                         Qt::AlignCenter);

    layout->addStretch();
    int hitWidth = 260;
    auto alignFlag = Qt::AlignLeft;
    QString hitsQSS = "QLabel{color:white; font-size:10px;}";

    QVBoxLayout *listlayout = new QVBoxLayout;
    listlayout->setContentsMargins(40, 0, 0, 0);

    textHits = new QLabel;
    textHits->setStyleSheet(hitsQSS);
    textHits->setAlignment(alignFlag);
    textHits->setFixedWidth(hitWidth);
    textHits->setWordWrap(true);
    textHits->setText(tr("1. Head to the Control Panel and click on Power Options"));
    listlayout->addWidget(textHits);
    listlayout->setAlignment(textHits, alignFlag);

    textHits = new QLabel;
    textHits->setAlignment(alignFlag);
    textHits->setStyleSheet(hitsQSS);
    textHits->setFixedWidth(hitWidth);
    textHits->setWordWrap(true);
    textHits->setText(tr("2. Click the link of the left - hand side \"Choose what the power buttons do\""));
    listlayout->addWidget(textHits);
    listlayout->setAlignment(textHits, alignFlag);

    textHits = new QLabel;
    textHits->setAlignment(alignFlag);
    textHits->setStyleSheet(hitsQSS);
    textHits->setFixedWidth(hitWidth);
    textHits->setWordWrap(true);
    textHits->setText(tr("3. Click the \"Change settings that are currently unavailable\" link"));
    listlayout->addWidget(textHits);
    listlayout->setAlignment(textHits, alignFlag);

    textHits = new QLabel;
    textHits->setAlignment(alignFlag);
    textHits->setStyleSheet(hitsQSS);
    textHits->setFixedWidth(hitWidth);
    textHits->setWordWrap(true);
    textHits->setText(tr("4. Untick the \"Turn on fast start - up(recommended)\" box under Shut - down settings and click on Save changes"));
    listlayout->addWidget(textHits);
    listlayout->setAlignment(textHits, alignFlag);

    layout->addLayout(listlayout);
    layout->addStretch();

    widget->setLayout(layout);
    return widget;
}

QWidget *MainWindow::InstallFailedBody()
{
    QString hits = "<p style='color:white; font-size:14px;'>" +
                   tr("Installation Failed.") +
                   "</p>";
    return HitsBodyWidget(FailIconURL, hits);
}

QWidget *MainWindow::InstallSuccessFooter()
{
    DPushButton *restartlater = new DPushButton(tr("Restart Later"));
    connect(restartlater, SIGNAL(clicked()), this, SLOT(close()));

    DPushButton *restartnow = new DPushButton(tr("Restart Now"));
    connect(restartnow, SIGNAL(clicked()), this, SLOT(reboot()));

    QList<DPushButton *> btlist;
    btlist.append(restartlater);
    btlist.append(restartnow);
    return new DFooterWidget(btlist);
}

void MainWindow::EnableCloseButton(bool enable)
{
    m_EnableClose = enable;
}

DHeaderWidget *MainWindow::Header()
{
    DHeaderWidget *m_Heaer = new DHeaderWidget;
    if (m_EnableClose) {
        connect(m_Heaer, SIGNAL(closeClicked()), this, SLOT(close()));
        m_Heaer->enableClose(true);
        return m_Heaer;
    }
    disconnect(m_Heaer, SIGNAL(closeClicked()), this, SLOT(close()));
    m_Heaer->enableClose(false);
    return m_Heaer;
}

void MainWindow::unistallClear()
{
    m_Backend->UninstallClear();
    close();
}

QWidget *MainWindow::FinishUnistallFooter()
{
    DPushButton *finish = new DPushButton(tr("Finished"));
    connect(finish, SIGNAL(clicked()), this, SLOT(unistallClear()));

    QList<DPushButton *> btlist;
    btlist.append(finish);
    return new DFooterWidget(btlist);
}

QWidget *MainWindow::FinishFooter()
{
    DPushButton *finish = new DPushButton(tr("Finished"));
    connect(finish, SIGNAL(clicked()), this, SLOT(close()));

    QList<DPushButton *> btlist;
    btlist.append(finish);
    return new DFooterWidget(btlist);
}

void MainWindow::goInstall()
{
    EnableCloseButton(true);

    m_AcceptKey = true;

    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(InstallOptionBody());
    if (m_DiskSizeEnough) {
        m_topLayout->addWidget(InstallFooter());
    } else {
        m_topLayout->addWidget(ExitFooter());
    }

    m_TopWidget->setLayout(m_topLayout);

}

void MainWindow::goReInstall()
{
    connect(m_Backend, SIGNAL(Done(int)), this, SLOT(reinstallDone(int)));
    goUninstallProcess();
    m_Backend->GoBack();
}

void MainWindow::goInstallOptionCheck()
{
    //checkusername
    QString err;
    bool ret = IsValidUsername(m_Username, err);
    if (!ret) {
        setUsername(m_Username);
        return;
    }
    //checkPassword
    if (m_Password.isEmpty()) {
        emit setPasswordTips(PasswordHits);
        emit showPasswordTips();
        return;
    }

    if (m_Password != m_RepeatPassword) {
        emit setRepeatPasswordTips(RepeatPasswordHits);
        emit showRepeatPasswordTips();
        return;
    }

    m_Backend->SetInstallParam(
        m_Username,
        QByteArray(m_Password.toUtf8()).toBase64(),
        m_InstallLocale,
        m_InstallDev,
        "",
        m_InstallSize
    );

    connect(m_Backend, SIGNAL(Done(int)), this, SLOT(installDone(int)));

    connect(m_Backend, SIGNAL(ActionUpdate(QString)),
            this, SLOT(updateActions(QString)));
    connect(m_Backend, SIGNAL(ProgressUpdate(int)),
            this, SLOT(updateProgress(int)));

    m_Backend->Go();
    m_AcceptKey = false;
    goInstallProcess();
}

void MainWindow::installDone(int ret)
{
    qDebug() << "installDone";
    if (Backend::Success == ret) {
        goInstallSuccess();
    } else {
        m_Backend->AsyncUninstall();
        goInstallFailed();
    }
    emit progressDone();
}

void MainWindow::uninstallDone(int ret)
{
    qDebug() << "uninstallDone";
    if (Backend::Success == ret) {
        goUninstallSuccess();
        return;
    }
    goUninstallFailed();
}

void MainWindow::reinstallDone(int ret)
{
    qDebug() << "reinstallDone";
    if (Backend::Success == ret) {
        goInstall();
        return;
    }
    goUninstallFailed();
}

void MainWindow::goInstallProcess()
{
    EnableCloseButton(false);

    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(InstallProcessBody());
    m_topLayout->addWidget(EmptyFooter());

    m_TopWidget->setLayout(m_topLayout);

}

void MainWindow::goInstallSuccess()
{
    EnableCloseButton(true);

    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(InstallSuccessBody());
    m_topLayout->addWidget(InstallSuccessFooter());

    m_TopWidget->setLayout(m_topLayout);

}

void MainWindow::goInstallFailed()
{
    EnableCloseButton(true);
    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(InstallFailedBody());
    m_topLayout->addWidget(FinishFooter());

    m_TopWidget->setLayout(m_topLayout);

}

QWidget *MainWindow::UninstallBody()
{
    QString hits = "<p style='color:white; font-size:14px;'>" +
                   QString(tr("You have installed %1.")).arg(m_Backend->Release()) +
                   "</p>";
    return HitsBodyWidget(WarningIconURL, hits);
}

QWidget *MainWindow::UninstallFooter()
{
    DPushButton *cancel = new DPushButton(tr("Cancel"));
    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

    DPushButton *uninstall = new DPushButton(tr("Uninstall"));
    connect(uninstall, SIGNAL(clicked()), this, SLOT(goUninstallPre()));

    DPushButton *reinstall = new DPushButton(tr("Reinstall"));
    connect(reinstall, SIGNAL(clicked()), this, SLOT(goReInstall()));

    QList<DPushButton *> btlist;
    btlist.append(cancel);
    btlist.append(uninstall);
    btlist.append(reinstall);
    return new DFooterWidget(btlist);
}

QWidget *MainWindow::UninstallProcessBody()
{
    QString hits = "<p style='color:white; font-size:14px;'>" +
                   tr("Being uninstalled. Please wait... ") +
                   "</p>";
    return HitsBodyWidget(WarningIconURL, hits);
}

QWidget *MainWindow::UninstallSuccessBody()
{
    QString hits = "<p style='color:white; font-size:14px;'>" +
                   tr("Uninstalled Successfully. ") +
                   "</p>";
    return HitsBodyWidget(SuccessIconURL, hits);
}

QWidget *MainWindow::UninstallFailedBody()
{
    QString hits = "<p style='color:white; font-size:14px;'>" +
                   tr("Uninstallation Failed.") +
                   "</p>";
    return HitsBodyWidget(FailIconURL, hits);
}

void MainWindow::goUninstall()
{
    EnableCloseButton(true);

    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(UninstallBody());
    m_topLayout->addWidget(UninstallFooter());

    m_TopWidget->setLayout(m_topLayout);

}

void MainWindow::goUninstallPre()
{
    connect(m_Backend, SIGNAL(Done(int)), this, SLOT(uninstallDone(int)));
    goUninstallProcess();
    m_Backend->GoBack();
}

void MainWindow::goUninstallProcess()
{
    EnableCloseButton(false);
    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(UninstallProcessBody());
    m_topLayout->addWidget(EmptyFooter());

    m_TopWidget->setLayout(m_topLayout);

}

void MainWindow::installDevTextChanged(const QString &devtext)
{
    int freeSize = devtext.right(devtext.length() - 3).split("GB").first().toInt();
    if (freeSize > (DefaultMaxInstallSize)) {
        freeSize = DefaultMaxInstallSize;
    }
    setInstallDev(devtext.left(2));
    emit maxInstallSizeChage(freeSize);
}

void MainWindow::installLanguageChanged(int index)
{
//    qDebug()<<"Set index"<<index<<"/"<<m_Languages.size();
    if ((index < 0) || (index >= m_Languages.size())) {
        index = 0;
    }

    m_InstallLocale = m_Languages.at(index).Locale;
}

void MainWindow::updateActions(const QString & /*act*/)
{
//   TODO: maybe tell user what we are doing
}

void MainWindow::updateProgress(int progress)
{
    emit progressChanged(progress);
}

void MainWindow::reboot()
{
    Xapi::Reboot();
    this->close();
}

void MainWindow::goUninstallSuccess()
{
    EnableCloseButton(true);

    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(UninstallSuccessBody());
    m_topLayout->addWidget(FinishUnistallFooter());

    m_TopWidget->setLayout(m_topLayout);

}

void MainWindow::goUninstallFailed()
{
    EnableCloseButton(true);

    QWidget *m_TopWidget = new QWidget(this);
    setCentralWidget(m_TopWidget);

    QVBoxLayout *m_topLayout = new QVBoxLayout();
    m_topLayout->addWidget(Header());
    m_topLayout->addWidget(UninstallFailedBody());
    m_topLayout->addWidget(FinishUnistallFooter());

    m_TopWidget->setLayout(m_topLayout);

}


void MainWindow::setUsername(const QString &value)
{
    QString v = value.toLower();
    QString err;
    if (!v.isEmpty()) {
        if (1 == v.length() && !IsValidUsernameFirstChar(v, err)) {
            v = v.left(value.length() - 1);
            emit setUsernameTips(err);
            emit showUsernameTips();
        } else {
            bool ret = IsValidUsernameChar(v.right(1), err);
            if (!ret) {
                v = v.left(value.length() - 1);
                //            QString newerr;
                //            if (!IsValidUsername(v, newerr)) {
                //                err = newerr;
                //            }
                emit setUsernameTips(err);
                emit showUsernameTips();
            } else if (!IsValidUsername(v, err)) {
                emit setUsernameTips(err);
                emit showUsernameTips();
            } else {
                emit hideUsernameTips();
            }
        }
    }

    m_Username = v;
    emit usernameChanged(m_Username);

}

void MainWindow::editUsernameBegin(const QString &v)
{
    if (v.isEmpty()) {
        emit hideUsernameTips();
    }
}

void MainWindow::editUsernameFinish()
{
    QString err;
    bool ret = IsValidUsername(m_Username, err);
    if (!ret) {
        emit setUsernameTips(err);
        emit showUsernameTips();
    } else {
        emit hideUsernameTips();
    }
}

void MainWindow::setPassword(const QString &v)
{
    m_Password = v;

    if (!m_RepeatPassword.isEmpty()) {
        if (m_Password != m_RepeatPassword) {
            emit setRepeatPasswordTips(RepeatPasswordHits);
            emit showRepeatPasswordTips();
        } else {
            emit hideRepeatPasswordTips();
        }
    } else {
        emit hideRepeatPasswordTips();
    }

    if (v.isEmpty()) {
        return;
    }

    emit hidePasswordTips();
}

void MainWindow::editPasswordBegin(const QString &v)
{
    if (v.isEmpty()) {
        emit hidePasswordTips();
    }
    //Check Repeat password
    if (m_Password != m_RepeatPassword) {
        emit setRepeatPasswordTips(RepeatPasswordHits);
        emit showRepeatPasswordTips();
    } else {
        emit hideRepeatPasswordTips();
    }
}

void MainWindow::editPasswordFinish()
{
    if (m_Password.isEmpty()) {
        emit setPasswordTips(PasswordHits);
        emit showPasswordTips();
    } else {
        emit hidePasswordTips();
    }
    //Check Repeat password
    if (m_RepeatPassword != m_Password) {
        emit setRepeatPasswordTips(RepeatPasswordHits);
        emit showRepeatPasswordTips();
    } else {
        emit hideRepeatPasswordTips();
    }
}

void MainWindow::setRepeatPassword(const QString &v)
{
    m_RepeatPassword = v;

    if (m_RepeatPassword.isEmpty()) {
        emit hideRepeatPasswordTips();
        return;
    }

    if (0 != m_Password.indexOf(m_RepeatPassword)) {
        emit setRepeatPasswordTips(RepeatPasswordHits);
        emit showRepeatPasswordTips();
    } else {
        emit hideRepeatPasswordTips();
    }
}

void MainWindow::editRepeatPasswordBegin(const QString &v)
{
    if (v.isEmpty()) {
        emit hideRepeatPasswordTips();
    }
    if (0 != m_Password.indexOf(m_RepeatPassword)) {
        emit setRepeatPasswordTips(RepeatPasswordHits);
        emit showRepeatPasswordTips();
    } else {
        emit hideRepeatPasswordTips();
    }
}

void MainWindow::editRepeatPasswordFinish()
{
    if (m_Password != m_RepeatPassword) {
        emit setRepeatPasswordTips(RepeatPasswordHits);
        emit showRepeatPasswordTips();
    } else {
        emit hideRepeatPasswordTips();
    }
}

void MainWindow::setInstallDev(const QString &v)
{
    m_InstallDev = v;
}

void MainWindow::setInstallSize(int v)
{
    m_InstallSize = v;
}
