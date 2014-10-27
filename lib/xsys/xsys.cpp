#include "xsys.h"

#include "xutil.h"

#include <QtCore>

#ifdef Q_OS_WIN32
#include <windows.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

static QString RunApp(const QString &execPath, const QString &execParam, const QString &execPipeIn="") {
    QString outPipePath = Xapi::TmpFilePath("pipeOut");

    QProcess app;
    app.setStandardInputFile(execPipeIn);
    app.setStandardOutputFile(outPipePath);
    app.setStandardErrorFile(outPipePath);
    app.start(execPath + " " + execParam);
    app.waitForFinished();

    if (QProcess::NormalExit != app.exitStatus())
        qDebug()<<app.error()<<app.errorString()<<app.state();

    QFile locale(outPipePath);
    if (!locale.open(QIODevice::ReadOnly)) {
        qDebug()<<"Open output pipe Fialed!"<<outPipePath;
        return "";
    }
    QTextStream localets(&locale);

    QString outUtf8PipePath = Xapi::TmpFilePath("utf8pipeOut");
    QFile utf8(outUtf8PipePath);
    if (!utf8.open(QIODevice::WriteOnly)) {
        qDebug()<<"Open utf8 output pipe Fialed!"<<outUtf8PipePath;
        return "";
    }
    QTextStream utf8ts(&utf8);
    utf8ts.setCodec("utf8");
    utf8ts<<localets.readAll();
    locale.close();
    utf8.close();

    utf8.open(QIODevice::ReadOnly);
    QString ret = QString(utf8.readAll());
    utf8.close();

    locale.remove();
    utf8.remove();
    return ret;
}

QString getGUID(const QString &mountPoint) {
    const size_t  maxGUIDPath = 50;
    wchar_t buffer[maxGUIDPath + 1];
    qDebug()<<"GetGUIDof: "<<mountPoint;
    GetVolumeNameForVolumeMountPoint(mountPoint.toStdWString().c_str(), buffer, maxGUIDPath);
    qDebug()<<"GetGUIDof: "<<QString("").fromStdWString(buffer).toLatin1()<< "  "<<GetLastError();
    return QString(QString("").fromStdWString(buffer).toLatin1()).remove("\\\\?\\Volume").remove("\\");
}


QString getUsername() {
    DWORD bufsize = 256;
    wchar_t buffer[256];
    GetUserName(buffer, &bufsize);
    return QString("").fromWCharArray(buffer, bufsize).toLatin1();
}

QString getHostname() {
    DWORD bufsize = 256;
    wchar_t buffer[256];
    GetComputerName(buffer, &bufsize);
    return QString("").fromWCharArray(buffer, bufsize).toLatin1();
}

QString getGMT() {
    TIME_ZONE_INFORMATION tz;
    GetTimeZoneInformation(&tz);

    LONG gmt = - tz.Bias / 60;
    if (gmt > 12 || gmt < -12) {
        gmt = 0;
    }
    return QString("%1").arg(gmt);
}

QString getCountry() {
    DWORD bufsize = 256;
    wchar_t buffer[256];
    GetLocaleInfo(LOCALE_USER_DEFAULT,
                  LOCALE_SISO3166CTRYNAME,
                  buffer,
                  bufsize);
    return QString("").fromWCharArray(buffer).toLatin1();
}


QString getTimezone() {
    return XUtils::StandTimezone(getCountry(), getGMT());
}

QString getKeyboardLayoutName() {
    wchar_t buffer[KL_NAMELENGTH + 1];
    GetKeyboardLayoutName(buffer);
    QString kbid = QString("").fromWCharArray(buffer).toLatin1();
    kbid = "0000" + kbid.right(4);
    return XUtils::StandKBLayout(kbid);
}

QString getKeyboardLayoutVariant() {
    wchar_t buffer[KL_NAMELENGTH + 1];
    GetKeyboardLayoutName(buffer);
    QString kbid = QString("").fromWCharArray(buffer).toLatin1();
    return XUtils::StandKBVariant(kbid);
}

QString getLocale() {
    DWORD bufsize = 256;
    wchar_t buffer[256];
    GetLocaleInfo(LOCALE_USER_DEFAULT,
                  LOCALE_SISO639LANGNAME,
                  buffer,
                  bufsize);

    QString lang = QString("").fromWCharArray(buffer).toLatin1();

    GetLocaleInfo(LOCALE_USER_DEFAULT,
                  LOCALE_SISO3166CTRYNAME,
                  buffer,
                  bufsize);

    QString country = getCountry();

    auto locale = lang + "_" + country;
    return XUtils::StandLoacle(locale);
}

#endif

#ifdef Q_OS_UNIX
static QString RunApp(const QString &execPath, const QString &execParam, const QString &execPipeIn="") {
    QProcess app;
    QString cmdline = execPath + " " + execParam;
    app.start(cmdline);
    if (!app.waitForStarted()) {
        qWarning()<<"Start app failed: "<<cmdline;
        return "";
    }

    app.write(execPipeIn.toLatin1());
    app.closeWriteChannel();

    if (!app.waitForFinished()) {
        qWarning()<<"App quit failed: "<<cmdline;
        return "";
    }

    return app.readAll();
}
#endif

class Execer: public QObject {
public:
    void run(const QString &execPipeIn="");
    QString ExecPath;
    QString Param;
    QString Ret;

};


void Execer::run(const QString &execPipeIn){
    Ret = RunApp(ExecPath, Param, execPipeIn);
    qDebug()<<"Exec: "<<ExecPath
            <<"Params: "<<Param
            <<"Output: "<<Ret;
}



namespace Xapi {

QString SynExec(const QString &exec, const QString &param, const QString &execPipeIn) {
    Execer execer;
    execer.ExecPath = exec;
    execer.Param = param;
    execer.run(execPipeIn);
    return execer.Ret;
}

QString RandString(const QString &str) {
    QString seedStr = str + QTime::currentTime().toString(Qt::SystemLocaleLongDate) + QString("%1").arg(qrand());
    return QString("").append(QCryptographicHash::hash(seedStr.toLatin1(), QCryptographicHash::Md5).toHex());
}

QString TmpFilePath(const QString &filename) {
    QString tmpDir = QStandardPaths::standardLocations(QStandardPaths::TempLocation)[0];
    QString ext =  + "." + filename.split(".").last();
    if ("." == ext || !filename.contains(".") ) {
        ext = "";
    }
    QString newFilename = RandString(filename);
//    qDebug()<<"New tmpFilename"<<newFilename;
    return QDir::toNativeSeparators(QString( tmpDir + "/"
            + newFilename + ext));
}

QString InsertTmpFile(const QString &fileurl) {
    QString filename = TmpFilePath(fileurl);
    QFile file(fileurl);
    file.open(QIODevice::ReadOnly);
    QString tmpPath = InsertFileData(filename, file.readAll());
    file.close();
    return tmpPath;
}

QString InsertTmpFileList(const QString &subdir, const QStringList &fileurlList) {
    QString tmpDir = TmpFilePath(subdir);
    QDir dir(tmpDir);
    dir.mkdir(".");

    foreach (QString fileurl, fileurlList) {
        QString filename = tmpDir + "/" + fileurl.split("/").last();
        QFile file(fileurl);
        file.open(QIODevice::ReadOnly);
        InsertFileData(filename, file.readAll());
        file.close();
    }
    return tmpDir;
}

QString InsertFile(const QString &fileurl, const QString &fullpath) {
    QFile file(fileurl);
    file.open(QIODevice::ReadOnly);
    QString tmpPath = InsertFileData(fullpath, file.readAll());
    file.close();
    return tmpPath;
}

QString InsertFileData(const QString &filename, const QByteArray &data) {
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
    qDebug()<<"Create File: "<<filename;
#ifdef Q_OS_UNIX
    SynExec("sync", "");
#endif
    return filename;
}

void RmFile(QFile &fn)
{
    if (!fn.exists()) return;
    fn.setPermissions(QFile::WriteUser);
    fn.remove();
#ifdef Q_OS_UNIX
    SynExec("sync", "");
#endif
}


void RmFile(const QString &filename)
{
    QFile file(filename);
    RmFile(file);
}

bool CpFile(const QString &srcName, const QString &desName) {
    bool ret = true;
    QFile srcFile(srcName);
    QFile desFile(desName);
    srcFile.open(QIODevice::ReadOnly);
    desFile.open(QIODevice::WriteOnly);
    QByteArray data = srcFile.readAll();
    qint64 writeBytes = desFile.write(data);
    if (writeBytes != data.size()) {
        qWarning()<<"Copy File Failed, "<<srcName<<" to "<<desName;
        ret = false;
    }
    srcFile.close();
    desFile.close();
#ifdef Q_OS_UNIX
    SynExec("sync", "");
#endif
    return ret;
}

bool RmDir(const QString &dirpath)
{
    bool result = true;
    QDir dir(dirpath);

    if (dir.exists(dirpath)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = RmDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirpath);
    }
#ifdef Q_OS_UNIX
    SynExec("sync", "");
#endif
    return result;
}

void MoveDir(const QString &oldName, const QString &newName)
{
    RmFile(newName);
    RmDir(newName);
    QDir dir(oldName);
    dir.rename(oldName, newName);
#ifdef Q_OS_UNIX
    SynExec("sync", "");
#endif
}

QString Username() {
    return getUsername();
}

QString Hostname() {
    return getHostname();
}

QString Locale() {
    return getLocale();
}


QString Timezone() {
    return getTimezone();
}

QString KeyboardLayoutName() {
    return getKeyboardLayoutName();
}

QString KeyboardLayoutVariant() {
    return getKeyboardLayoutVariant();
}

QString GetFileMD5(const QString &filePath) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile in(filePath);
    QFileInfo fileInfo(filePath);
    qint64 fileSize = fileInfo.size();

    const qint64 bufferSize = 1024*16;
    if (in.open(QIODevice::ReadOnly)) {
        char buf[bufferSize];
        int bytesRead;

        int readSize = qMin(fileSize, bufferSize);
        while (readSize > 0 && (bytesRead = in.read(buf, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buf, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        in.close();
        return hash.result().toHex();
    }
    qDebug() << "Failed to open device!";
    return "";
}

void Reboot() {
#ifdef Q_OS_WIN32
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    ExitWindowsEx(EWX_REBOOT, EWX_FORCE);
#endif
#ifdef Q_OS_LINUX
    m_UnetbootinPtr->callexternapp("init", "6 &");
#endif
#ifdef Q_OS_MAC
    unetbootinPtr->callexternapp("shutdown", "-r now &");
#endif
}


#include <intrin.h>

Arch CpuArch () {
#ifdef Q_OS_WIN32
    int CPUInfo[4];
    __cpuid(CPUInfo, 0);
    return  ((CPUInfo[3] & 0x20000000) || false) ? AMD64 : X86;
#endif
}


}
