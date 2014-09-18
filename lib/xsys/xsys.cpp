#include "xsys.h"

#include "xutil.h"

#include <QtCore>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "Advapi32.lib")

static QString RunApp(const QString &execPath, const QString &execParam, const QString &execPipeIn="") {
    SECURITY_ATTRIBUTES sa={sizeof(sa),NULL,TRUE};
    SECURITY_ATTRIBUTES *psa=&sa;
    DWORD dwShareMode=FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;
    OSVERSIONINFO osVersion={0};
    osVersion.dwOSVersionInfoSize=sizeof(osVersion);

//    if(GetVersionEx(&osVersion)){
//        if(osVersion.dwPlatformId==VER_PLATFORM_WIN32_NT){
//        psa=&sa;
//        dwShareMode|=FILE_SHARE_DELETE;
//        }
//    }

    QString outPipePath = Xapi::TmpFilePath("pipeOut");
    HANDLE hConsoleCoutRedirect=CreateFile(
        LPWSTR(outPipePath.utf16()),
        GENERIC_WRITE,
        dwShareMode,
        psa,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );

    if(hConsoleCoutRedirect==INVALID_HANDLE_VALUE)
    {
        qWarning()<<"cout error"<<GetLastError();
    };

    QString inPipePath = Xapi::TmpFilePath("pipeIn");
    QFile inFile(inPipePath);
    inFile.open(QIODevice::WriteOnly);
    inFile.write(execPipeIn.toLatin1());
    inFile.close();

    HANDLE hConsoleCinRedirect=CreateFile(
            LPWSTR(inPipePath.utf16()),
            GENERIC_READ,
            dwShareMode,
            psa,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
            );

    STARTUPINFO s={sizeof(s)};
    s.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
    s.hStdOutput=hConsoleCoutRedirect;
    s.hStdError=hConsoleCoutRedirect;
    s.hStdInput=hConsoleCinRedirect;
    s.wShowWindow=SW_HIDE;
    s.lpReserved = NULL;
    s.lpDesktop = NULL;
    s.lpTitle = NULL;
    s.wShowWindow = SW_HIDE;
    s.cbReserved2 = NULL;
    s.lpReserved2 = NULL;
    PROCESS_INFORMATION pi={0};

    QString cmdline = execPath + " " + execParam;

    if(CreateProcess(NULL,LPWSTR(cmdline.utf16()),NULL,NULL,TRUE,NULL,NULL,NULL,&s,&pi))
    {
        WaitForSingleObject(pi.hProcess,INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        CloseHandle(hConsoleCoutRedirect);
        CloseHandle(hConsoleCinRedirect);
    }

    QFile outFile(outPipePath);
    outFile.open(QIODevice::ReadOnly);
    QString ret = outFile.readAll();
    outFile.close();

    inFile.remove();
    outFile.remove();

    return ret.toLatin1();
}

QString getDeviceGUID(const QString &device){
    DWORD  CharCount            = 0;
    WCHAR  DeviceName[MAX_PATH] = L"";
    DWORD  Error                = ERROR_SUCCESS;
    HANDLE FindHandle           = INVALID_HANDLE_VALUE;
    size_t Index                = 0;
    BOOL   Success              = FALSE;
    WCHAR  VolumeName[MAX_PATH] = L"";

    //
    //  Enumerate all volumes in the system.
    FindHandle = FindFirstVolume(VolumeName, ARRAYSIZE(VolumeName));

    if (FindHandle == INVALID_HANDLE_VALUE)
    {
        Error = GetLastError();
        wprintf(L"FindFirstVolumeW failed with error code %d\n", Error);
        return "";
    }

    for (;;)
    {
        //
        //  Skip the \\?\ prefix and remove the trailing backslash.
        Index = wcslen(VolumeName) - 1;

        if (VolumeName[0]     != L'\\' ||
            VolumeName[1]     != L'\\' ||
            VolumeName[2]     != L'?'  ||
            VolumeName[3]     != L'\\' ||
            VolumeName[Index] != L'\\')
        {
            Error = ERROR_BAD_PATHNAME;
            wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
            break;
        }

        //
        //  QueryDosDeviceW does not allow a trailing backslash,
        //  so temporarily remove it.
        VolumeName[Index] = L'\0';
//{bdeb8709-b930-473e-a135-878cbdb820f1}
        //"Volume{6def5e48-f5ef-4e77-b02d-4fe2b784966b}"
        qDebug()<<QString("").fromWCharArray(&VolumeName[4]).toLatin1();
        CharCount = QueryDosDevice(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));

        VolumeName[Index] = L'\\';

        if ( CharCount == 0 )
        {
            Error = GetLastError();
            wprintf(L"QueryDosDeviceW failed with error code %d\n", Error);
            break;
        }

        QString deviceName = QString("").fromStdWString(DeviceName).toLatin1();
        qDebug()<<QString("").fromStdWString(VolumeName).toLatin1();
        qDebug()<<deviceName;
        if (device == deviceName) {
            return QString(QString("").fromStdWString(VolumeName).toLatin1()).remove("\\\\?\\Volume").remove("\\");
        }
        //
        //  Move on to the next volume.
        Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));

        if ( !Success )
        {
            Error = GetLastError();

            if (Error != ERROR_NO_MORE_FILES)
            {
                wprintf(L"FindNextVolumeW failed with error code %d\n", Error);
                break;
            }

            //
            //  Finished iterating
            //  through all the volumes.
            Error = ERROR_SUCCESS;
            break;
        }
    }

    FindVolumeClose(FindHandle);
    FindHandle = INVALID_HANDLE_VALUE;
    return "";
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
    QSettings setting("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout\\DosKeybCodes\\", QSettings::NativeFormat);
    return setting.value(kbid).toString();
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
    qDebug()<<"New tmpFilename"<<newFilename;
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

    for (auto fileurl: fileurlList) {
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


QString PartitionGUID(const QString &mountPoint) {
    return getDeviceGUID(mountPoint);
}

QString Username() {
    return getUsername();
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

}
