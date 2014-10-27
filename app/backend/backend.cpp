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

extern QString ToDeepinUsername(const QString& username);
extern QString ToDeepinHostname(const QString& username);

static QString BlobAppSevenZ = "sevnz.exe";

Backend::Backend(const QString &username,
        const QString &password,
        const QString& installTarget,
        const QString& isoPath,
        int installSize,
        QObject *parent):QObject(parent){
    Init(username, password, installTarget, isoPath, installSize);

    QStringList sevenzFileList;
    sevenzFileList.append(":/blobs/sevenz/sevnz.exe");
    sevenzFileList.append(":/blobs/sevenz/7z.dll");
    Xapi::GetBlobs().Install(BlobAppSevenZ, sevenzFileList);
}

void Backend::SetInstallParam(
        const QString &username,
        const QString &password,
        const QString& installTarget,
        const QString& isoPath,
        int installSize) {
    Init(username, password, installTarget, isoPath, installSize);
}

void Backend::Init(
        const QString &username,
        const QString &password,
        const QString& installTarget,
        const QString& isoPath,
        int installSize) {
    m_Info.TargetDev = installTarget.left(2);
    m_Info.InstallPrefix = "deepin";
    m_Info.InstallPath = m_Info.TargetDev + "/" + m_Info.InstallPrefix;
    m_Info.KernelPath = m_Info.InstallPath + "/install/boot/vmlinuz";
    m_Info.InitrdPath = m_Info.InstallPath + "/install/boot/initrd.lz";
    m_Info.ImagePath = isoPath;
    m_Info.DistroName = "Deepin";
    //2GB for reseve
    m_Info.RootSize = installSize * 1024 - 2.5 * 1024;
    m_Info.SwapSize = 0.5 * 1024;
    m_Info.RootFlags = "rootflags=sync";
    m_Info.RootFilePath = QString("/%1/disks/root.disk").arg(m_Info.InstallPrefix);
    m_Info.SwapFilePath = QString("/%1/disks/swap.disk").arg(m_Info.InstallPrefix);
    m_Info.KeyboardVariant = Xapi::KeyboardLayoutVariant();
    m_Info.KeyboardLayout = Xapi::KeyboardLayoutName();
    m_Info.Locale =Xapi::Locale();

    m_Info.Username = ToDeepinUsername(username);
    m_Info.Password = password;
    m_Info.Hostname = ToDeepinHostname(Xapi::Hostname());
    m_Info.Timezone = Xapi::Timezone();
    m_Info.Version = Version;
    m_Info.ReleaseInfo = "Deepin";

    m_Progress=0;
}

int Backend::Go(){
    QThread *worker = new QThread();;
    this->moveToThread(worker);
    worker->start();

    connect(this, SIGNAL(Install()), this, SLOT(AsyncInstall()));
    emit Install();

    return Success;
}

int Backend::GoBack(){
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
    FunctionLoger<int> log("AsyncInstall", ret);
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
    installStructure.append("deepin/disks");
    installStructure.append("deepin/install");
    installStructure.append("deepin/install/boot");
    installStructure.append("deepin/install/boot/grub");
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
            + " -i!" + "casper/vmlinuz"
            + " -o" + m_Info.InstallPath + "/install/boot "
            + "\"" + imagePath +  "\"";
    Xapi::SynExec(sevenz, cmdline);

    this->Increment (4);

    cmdline = QString(" e -y ")
            + " -i!" + "casper/initrd.lz "
            + " -o" + m_Info.InstallPath + "/install/boot "
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
        qDebug()<<"Insert"<<info.right (info.length () -1 - 32);
        m_MD5Map.insert (info.right (info.length () -1 - 32), info.left (32));
    }

    m_DownloadFinsh = true;
}

void Backend::FetchMD5List() {
    qDebug()<<"FetchMD5List";
    QString url = "http://cdimage.linuxdeepin.com/releases/MD5SUMS";
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

    qDebug()<<"Open File info";
    QFile diskdefine(m_Info.InstallPath + "/install/boot/info");
    diskdefine.open (QIODevice::ReadOnly);
    QString content = QString(diskdefine.readAll()).split ("\n").first ();
    diskdefine.close ();
    return content.remove ("\r");

    // TODO: fiter 32 bit cpu
//    QStringList releaselist = content.split ("\n").filter ("#define DISKNAME ").filter ("deepin");

////    Xapi::Arch arch = Xapi::CpuArch ();
////    if (Xapi::X86 == arch) {
////        releaselist = releaselist.filter ("i386");
////    }
//    if (releaselist.isEmpty ()) {
//        return "";
//    }
//    return releaselist.first().remove("#define DISKNAME ");
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
    QStringList imagelist = curDir.entryList (fiter);
    qDebug()<<"Find"<<imagelist.length ()<<"image file";

    QStringList::iterator itor = imagelist.begin ();

    //Begin calc progress
    int totalProgress = 20;
    int totalImageNum = imagelist.length ();
    m_BasePersent = 10;
    ProgressReporter<Backend> *pr = new ProgressReporter<Backend>(this);
    for(;itor != imagelist.end (); ++itor) {
        int progressslice = totalProgress / totalImageNum;
        qDebug()<<"Find Image File"<<*itor;
        QString release = GetRelesaseVersion(*itor);
        if (release.isEmpty ()) {
            qDebug()<<"Find Image empty release"<<release;
            continue;
        }

        qDebug()<<"Find Image release"<<release;
        QString md5 = m_MD5Map[release];
        qDebug()<<"Check md5"<<md5;
        if (!md5.isEmpty ()) {
            m_BaseRange = progressslice;
            if (md5 != GetFileMD5 (*itor, pr)) {
                continue;
            }
            m_BasePersent += progressslice;
            qDebug()<<"Check md5 pass"<<md5;
        }

        m_Info.ReleaseInfo = release;
        return *itor;
    }
    return "";
}






}
