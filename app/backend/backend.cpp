#include "backend.h"

#include "../config/log.h"

#include <xsys.h>

#include <QDir>
#include <QApplication>
#include <QThread>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QTime>
#include <QMap>

#include "utils.h"

namespace DeepinInstaller{

const QString AppTitle() {
    return QObject::tr("Deepin System Installer");
}

extern QString ToDeepinUsername(const QString& username);
extern QString ToDeepinHostname(const QString& username);

static QString BlobAppSevenZ = "sevnz.exe";

Backend::Backend(const QString &username,
        const QString &password,
        const QString &locale,
        const QString& installTarget,
        const QString& isoPath,
        int installSize,
        int swapSize,
        QObject *parent):QObject(parent){
    Init(username, password, locale, installTarget, isoPath, installSize, swapSize);

    QStringList sevenzFileList;
    sevenzFileList.append(":/blobs/sevenz/sevnz.exe");
    sevenzFileList.append(":/blobs/sevenz/7z.dll");
    Xapi::GetBlobs().Install(BlobAppSevenZ, sevenzFileList);
}

void Backend::SetInstallParam(
        const QString &username,
        const QString &password,
        const QString &locale,
        const QString& installTarget,
        const QString& isoPath,
        int installSize,
        int swapSize) {
    Init(username, password, locale, installTarget, isoPath, installSize, swapSize);
}

void Backend::Init(
        const QString &username,
        const QString &password,
        const QString &locale,
        const QString& installTarget,
        const QString& isoPath,
        int installSize,
        int swapSize) {
    m_Info.TargetDev = installTarget.left(2);
    m_Info.InstallPrefix = "deepin";
    m_Info.BootMethod = "casper";
    m_Info.InstallPath = m_Info.TargetDev + "/" + m_Info.InstallPrefix;
    m_Info.KernelPath = m_Info.InstallPath + "/install/boot/vmlinuz";
    m_Info.InitrdPath = m_Info.InstallPath + "/install/boot/initrd.lz";
    m_Info.ImagePath = isoPath;
    m_Info.DistroName = "Deepin";
    //2GB for reseve
    m_Info.RootSize = installSize * 1024;
    m_Info.SwapSize = swapSize;
    m_Info.RootFlags = "rootflags=sync";
    m_Info.RootFilePath = QString("/%1/disks/root.disk").arg(m_Info.InstallPrefix);
    m_Info.SwapFilePath = QString("/%1/disks/swap.disk").arg(m_Info.InstallPrefix);
    m_Info.KeyboardVariant = Xapi::KeyboardLayoutVariant();
    m_Info.KeyboardLayout = Xapi::KeyboardLayoutName();
    m_Info.Locale = locale;

    m_Info.Username = ToDeepinUsername(username);
    m_Info.Password = password;
    m_Info.Hostname = ToDeepinHostname(Xapi::Hostname());
    m_Info.Timezone = Xapi::Timezone();
    m_Info.Version = Version;
    m_Info.ReleaseInfo = "Deepin";

    m_Progress=0;

    qDebug() << property("BootMode");
}

int Backend::Go(){
    qDebug()<<QThread::currentThread();
    QThread *worker = new QThread();;
    this->moveToThread(worker);
    worker->start();

    connect(this, SIGNAL(Install()), this, SLOT(AsyncInstall()));
    emit Install();

    return Success;
}

int Backend::GoBack(){
    qDebug()<<QThread::currentThread();
    QThread *worker = new QThread();;
    this->moveToThread(worker);
    worker->start();

    connect(this, SIGNAL(Uninstall()), this, SLOT(AsyncUninstall()));

    emit Uninstall();

    return Success;
}

void Backend::AsyncInstall() {
    int ret = Failed;
    FunctionLoger<int> log("AsyncInstall", ret);

    QList<InstallAction> actions;
    actions.push_back(&Backend::CreateInstallDir);
    actions.push_back(&Backend::CreateUninstaller);
    actions.push_back(&Backend::FetchISO);
    actions.push_back(&Backend::ExtractISO);
    actions.push_back(&Backend::CreateVirtualDisks);
    actions.push_back(&Backend::CreateConfig);
    actions.push_back(&Backend::InstallBootloader);
    actions.push_back(&Backend::InstallGrub);
    actions.push_back(&Backend::MigrationData);

    foreach(InstallAction action, actions) {
        ret = (this->*action)();
        if (Failed == ret) {
            break;
        }
    }
    emit Done(ret);
}

void Backend::AsyncUninstall() {
    int ret = Failed;
    FunctionLoger<int> log("AsyncUninstall", ret);
    ret = this->UninstallApp();
    emit Done(ret);
}

int Backend::CreateInstallDir() {
    int ret = Failed;
    FunctionLoger<int> log("CreateInstallDir", ret);

    this->SetAction ("CreateInstallDir");
    this->Increment (1);

    QStringList installStructure;
    installStructure.append("deepin");
    installStructure.append("deepin/data"); // TODO: remove
    installStructure.append("deepin/data/backup"); // TODO: remove
    installStructure.append("deepin/disks");
    installStructure.append("deepin/install");
    installStructure.append("deepin/install/boot");
    installStructure.append("deepin/install/data");
    installStructure.append("deepin/install/hook");
    installStructure.append("deepin/install/boot/grub");
    installStructure.append("deepin/install/hook/migration");
    installStructure.append("deepin/install/data/migration");
    installStructure.append("deepin/winboot");

    qDebug()<<"Install to "<<m_Info.TargetDev;

    bool result = true;
    QDir root(m_Info.TargetDev);
    foreach(QString v, installStructure) {
        if(!root.exists(v))
            result &= root.mkdir(v);
    }
    this->Increment (4);
    return ret = result ? Success : Failed;
}

int Backend::ExtractISO() {
    int ret = Failed;
    FunctionLoger<int> log("ExtractISO", ret);

    this->SetAction ("ExtractISO");
    this->Increment (2);
    // Extract Kernel and Initrd for iso
    QString imagePath = m_Info.InstallPath + "/install/install.iso";
    QString sevenz = Xapi::GetBlobs().Get(BlobAppSevenZ);

    QString cmdline = QString(" e -y ")
            + " -i!" + "md5sum.txt"
            + " -o" + m_Info.InstallPath + "/install/boot "
            + "\"" + imagePath +  "\"";
    Xapi::SynExec(sevenz, cmdline);

    /*Check iso version*/
    QString fileListPath = m_Info.InstallPath + "/install/boot/md5sum.txt";
    QFile md5File(fileListPath);
    if (!md5File.open(QIODevice::ReadOnly)) {
        qWarning()<<"Open File Failed: "<< fileListPath;
    }
    QString fileListData = md5File.readAll();
    m_Info.BootMethod = "casper";
    if (fileListData.contains("/live/vmlinuz")) {
        m_Info.BootMethod = "live";
    }

    cmdline = QString(" e -y ")
            + " -i!" + m_Info.BootMethod + "/vmlinuz"
            + " -o" + m_Info.InstallPath + "/install/boot "
            + "\"" + imagePath +  "\"";
    Xapi::SynExec(sevenz, cmdline);

    this->Increment (4);

    cmdline = QString(" e -y ")
            + " -i!" + m_Info.BootMethod + "/initrd.lz "
            + " -o" + m_Info.InstallPath + "/install/boot "
            + "\"" + imagePath +  "\"";
    Xapi::SynExec(sevenz, cmdline);

    cmdline = QString(" e -y ")
            + " -i!" + ".hook/migration/ "
            + " -o" + m_Info.InstallPath + "/install/hook/migration "
            + "\"" + imagePath +  "\"";
    Xapi::SynExec(sevenz, cmdline);

    this->Increment (4);
    return Success;
}

int Backend::Progress() const{
    return m_Progress;
}

const QString& Backend::Action() const{
    return m_CurAction;
}

const QString& Backend::Error() const {
    return m_LastError;
}

/*
    int CreateUninstaller() = 0;

    int CreateVirtualFile() = 0;

    int CreatePressed() = 0;

    int InstallBootloader() = 0;

    int InstallGrub() = 0;
};*/

QString Backend::Release() {
    QString release;
    QStringList releaseInfo = m_Info.ReleaseInfo.split (" ");
    for (int i = 0; i < 2 && i <releaseInfo.length (); ++i) {
        release += releaseInfo[i] + " ";
    }
    if (release.isEmpty ()) {
        return release;
    }

    return release.left (release.length () - 1);
}

void Backend::Report (quint64 total, quint64 copyed){
     m_Progress= m_BasePersent + copyed * m_BaseRange /total;
     emit ProgressUpdate (m_Progress);
}

void Backend::DownloadMD5ListFinish (){
    if(m_Reply->error() != QNetworkReply::NoError) {
        qDebug()<<"Download MD5 list failed"<<m_Reply->error()
                <<m_Reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    }
    qDebug()<<"Download MD5 list end";

    QString imagemd5 = m_Reply->readAll();

    //append local file
    QFile isolist(":/data/imagemd5.list");

    if (!isolist.open(QIODevice::ReadOnly)) {
        qDebug()<<"Open imagemd5.list failed";
    }

    imagemd5 += isolist.readAll ();

    QStringList list = imagemd5.split("\n").filter ("Deepin");
    foreach(QString info, list) {
        QString md5 = info.split(" ").first ();
        QString rpath = info.split(" ")[1];
        QString release = info.remove (md5 + " ").remove (rpath + " ");
//        release = release.left (release.length () - 1);
        qDebug()<<"Insert"<<release<<md5;
        m_MD5Map.insert (release, md5);
    }

    m_DownloadFinsh = true;
}

void Backend::FetchMD5List() {
    qDebug()<<"FetchMD5List";
    QString url = "http://cdimage.inuxdeepin.com/releases/MD5SUMS";
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    m_DownloadFinsh = false;
    m_Reply = manager->get(QNetworkRequest(url));

    connect (m_Reply, SIGNAL(finished()), this, SLOT(DownloadMD5ListFinish()));
}

QString Backend::GetRelesaseVersion(const QString& imagePath){
    QString sevenz = Xapi::GetBlobs().Get(BlobAppSevenZ);

    QString cmdline = QString(" e -y ")
        + " -i!" + ".disk/info"
        + " -o" + m_Info.InstallPath + "/install/boot "
        + "\"" + imagePath + "\"";
    Xapi::SynExec(sevenz, cmdline);

    QString infoPath = QDir::toNativeSeparators(m_Info.InstallPath + "/install/boot/info");
    QFile diskdefine(infoPath);
    if (!diskdefine.open (QIODevice::ReadOnly)) {
        qDebug()<<"Open info failed"<<infoPath<<diskdefine.errorString();
        return "";
    }
    QString content = QString(diskdefine.readAll());
    diskdefine.close ();

    Xapi::Arch arch = Xapi::CpuArch ();
    if (Xapi::X86 == arch && !content.contains("i386")) {
        qDebug()<<"Unsuport arch" << arch << "Skip iso"<<content;
        return "";
    }

    if (!content.contains("Deepin")) {
        qDebug()<<"Not Deepin System Image,Skip iso"<<content;
        return "";
    }

    return content.remove ("\r").remove("\n");
}

QString Backend::FetchImageFiles() {
    //Get all iso file of cur dir
    FetchMD5List();

    if (!m_DownloadFinsh) {
        QTime dieTime = QTime::currentTime().addMSecs(1000);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    qDebug()<<"DownloadMD5ListFinish end";

    QString curDirPath = QApplication::applicationDirPath ();
    QDir curDir(curDirPath);
    QStringList fiter;
    fiter.push_back ("*.iso");
    QFileInfoList imagelist = curDir.entryInfoList (fiter);
    qDebug()<<"Find"<<imagelist.length ()<<"image file";

    QFileInfoList::iterator itor = imagelist.begin ();

    //Begin calc progress
    int totalProgress = 20;
    int totalImageNum = imagelist.length ();
    m_BasePersent = 10;
    ProgressReporter<Backend> *pr = new ProgressReporter<Backend>(this);
    for(;itor != imagelist.end (); ++itor) {
        int progressslice = totalProgress / totalImageNum;
        qDebug()<<"Find Image File"<<itor->absoluteFilePath();
        QString release = GetRelesaseVersion(itor->absoluteFilePath());
        if (release.isEmpty ()) {
            qDebug()<<"Find Image empty release"<<release;
            continue;
        }

        qDebug()<<"Find Image release"<<release;
        QString md5 = m_MD5Map[release];
        qDebug()<<"Check md5"<<md5;
        if (!md5.isEmpty ()) {
            m_BaseRange = progressslice;
            if (md5 != GetFileMD5 (itor->absoluteFilePath(), pr)) {
                continue;
            }
            m_BasePersent += progressslice;
            qDebug()<<"Check md5 pass"<<md5;
        }

        m_Info.ReleaseInfo = release;
        return itor->absoluteFilePath();
    }
    return "";
}






}
