#include "winbackend.h"
#include "utils.h"

#include "../config/log.h"

#include <xsys.h>

#include <Firmware>

#include <windows.h>

#include <QFile>
#include <QDir>
#include <QSettings>
#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QStandardPaths>
#include <QProcess>

using namespace DeepinInstaller;

static const QString UninstallRegistryKey = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
static const QString AppName = "DeepinSystemInstaller";
static const QString BootloaderKey = "VistaBootDrive";
static const QString UninstallString = "UninstallString";
static const QString RegistryKey = UninstallRegistryKey + AppName;

static QString BlobAppMkisofs = "mkisofs.exe";

enum WindowsVersion {
    WinUnknow,
    Win2000,
    WinXP,
    WinVista,
    WinSeven,
    WinEight,
};

// TODO: use VerifyVersionInfo
#pragma warning (disable : 4996)

int GetWindowsVersion()
{
    OSVERSIONINFOEX os;
    os.dwOSVersionInfoSize = sizeof(os);
    if (!GetVersionEx((OSVERSIONINFO *)&os)) {
        return WinUnknow;
    }

    switch (os.dwMajorVersion) {
    case 5:
        switch (os.dwMinorVersion) {
        case 0:
            return Win2000;
        case 1:
            return WinXP;
        case 2:
            //TODO: XP 64Bit, 2003, 2003R2
            return WinUnknow;
        }
        break;
    case 6:
        switch (os.dwMinorVersion) {
        case 0:
            switch (os.wProductType) {
            case VER_NT_WORKSTATION:
                return WinVista;
            default:
                //Microsoft Windows Server 2008
                return WinUnknow;
            }
            break;
        case 1:
            switch (os.wProductType) {
            case VER_NT_WORKSTATION:
                return WinSeven;
            default:
                //Microsoft Windows Server 2008 R2
                return WinUnknow;
            }
            break;
        case 2:
            return WinEight;
        }
        break;
    default:
        return WinUnknow;
    }
    return WinUnknow;
}

BootloaderType BCDType()
{
    QString bcdedit = Xapi::SystemDirtory() + "\\bcdedit.exe";
    QString cmd = QString("\"%1\" /enum").arg(bcdedit);
    QString ret =  Xapi::SynExec(cmd, "");
    ret = ret.toLower();

    QStringList bootEntryList = ret.split("-------------------").filter("bootmgr");
    foreach (QString entry, bootEntryList) {
        if (entry.contains("bootmgfw.efi")) {
            return BCD_UEFI;
        }
    }
    return BCD_BIOS;
}

BootloaderType WindowsBootLoaderType()
{
    //Test for win7 first
    switch (GetWindowsVersion()) {
    case WinSeven:
    case WinEight:
        //Detect if uefi Install
        return BCDType();
    case WinXP:
        return BOOT_INI;
    default:
        return BL_UNKNOW;
    }
}

int ESPVolnameIndex()
{
    QString bcdedit = Xapi::SystemDirtory() + "\\bcdedit.exe";
    QString cmd = QString("\"%1\" /enum").arg(bcdedit);
    QString ret =  Xapi::SynExec(cmd, "");

    QString bootmgr = ret.split("-------").filter("bootmgr").first();
    QString device = bootmgr.split("\n").filter("partition=\\Device").first();
    device = device.split("=").last();
    device = device.remove("\\Device\\HarddiskVolume");
    return device.toInt();
}

int DeviceType(const QString &target)
{
    return GetDriveType(target.toStdWString().c_str());
}

QString ToRelativePath(const QString &windowsPath)
{
    QString rpath = windowsPath;
    rpath.replace("\\", "/");
    if ((rpath.length() > 1) && (':' == rpath[1])) {
        rpath = rpath.right(rpath.length() - 2);
    }

    if ((rpath.length() > 1) && ("/" == rpath.right(1))) {
        rpath = rpath.left(rpath.length() - 1);
    }
    return rpath;
}

bool CreateVirtualFile(const QString &filePath, quint64 size)
{
    HANDLE priHandle;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &priHandle)) {
        qDebug() << "OpenProcessToken  Failed.";
    }

    LUID luid;
    if (!LookupPrivilegeValue(NULL, SE_MANAGE_VOLUME_NAME, &(luid))) {
        qDebug() << "LookupPrivilegeValue Failed.";
    }

    TOKEN_PRIVILEGES token;
    token.PrivilegeCount = 1;
    token.Privileges[0].Luid = luid;
    token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (! AdjustTokenPrivileges(priHandle, FALSE, &(token), 0, NULL, NULL)) {
        qDebug() << "AdjustTokenPrivileges Failed.";
    }
    CloseHandle(priHandle);

    HANDLE handle = CreateFile(
                        filePath.toStdWString().c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed open file" << filePath;
        return false;
    }

    quint64 fileBytes = size * 1024 * 1024;
    LARGE_INTEGER filePos;
    filePos.QuadPart = fileBytes;

    if (!SetFilePointerEx(handle, filePos, 0, FILE_BEGIN)) {
        qDebug() << "Failed SetFilePointerEx" << filePath << fileBytes;
        return false;
    }

    if (!SetEndOfFile(handle)) {
        qDebug() << "Failed SetEndOfFile" << filePath;
        return false;
    }

    SetFileValidData(handle, filePos.QuadPart);

    filePos.QuadPart = 0;
    if (!SetFilePointerEx(handle, filePos, 0, FILE_BEGIN)) {
        qDebug() << "Failed SetFilePointerEx 0" << filePath;
        return false;
    }

    //zero File
    quint64 bufSize = 1024 * 32;
    quint8 *writeBuf = new quint8[bufSize];
    quint64 writeBufSize = bufSize;
    memset(writeBuf, NULL, writeBufSize);
    quint64 clearBytes = 1000000;
    quint64 byteCleared = 0;
    quint64 bytesWrite = writeBufSize;
    DWORD bytesWrited = 0;
    byteCleared += bytesWrited;
    while (byteCleared < clearBytes) {
        bytesWrite = writeBufSize;
        if (bytesWrite > (fileBytes - byteCleared)) {
            bytesWrite = fileBytes - byteCleared;
        }

        bool ret = WriteFile(handle, writeBuf, bytesWrite, &bytesWrited, NULL);
        if (!ret || (0 == bytesWrited)) {
            qDebug() << "Failed WriteFile " << filePath;
            return false;
        }

        byteCleared += bytesWrited;
    }

    filePos.QuadPart = fileBytes - clearBytes;
    if (!SetFilePointerEx(handle, filePos, 0, FILE_BEGIN)) {
        qDebug() << "Failed SetFilePointerEx 0" << filePath;
        return false;
    }

    byteCleared = 0;
    writeBufSize = bufSize;
    bytesWrite = writeBufSize;
    bytesWrited = 0;
    while (byteCleared < clearBytes) {
        bytesWrite = writeBufSize;
        if (bytesWrite > (fileBytes - byteCleared)) {
            bytesWrite = fileBytes - byteCleared;
        }

        bool ret = WriteFile(handle, writeBuf, bytesWrite, &bytesWrited, NULL);
        if (!ret || (0 == bytesWrited)) {
            qDebug() << "Failed WriteFile " << filePath;
            return false;
        }

        byteCleared += bytesWrited;
    }

    delete[] writeBuf;
    CloseHandle(handle);
    return true;
}

int UnpackBootXXXX(const QString &bootxxxx);


WindowsBackend::WindowsBackend(
    const QString &username,
    const QString &locale,
    const QString &password,
    const QString &installTarget,
    const QString &isoPath,
    int installSize,
    QObject *parent):
    Backend(username, password, locale, installTarget, isoPath, installSize, parent)
{
    m_BasePersent = 0;
    m_BaseRange = 10;

    qDebug() << Xapi::SystemDirtory();

    QSettings settings(RegistryKey, QSettings::NativeFormat);
    m_Info.ReleaseInfo = settings.value("ReleaseInfo").toString();
    if (m_Info.ReleaseInfo.isEmpty()) {
        m_Info.ReleaseInfo = "Deepin";
    }

    QStringList mkisofsFileList;
    mkisofsFileList.append(":/blobs/tools/mkisofs.exe");
    Xapi::GetBlobs().Install(BlobAppMkisofs, mkisofsFileList);
}

int WindowsBackend::UninstallClear()
{
    //copy tmp file and remove
    qDebug() << "Is m_Uninstall" << m_isUninstall;
    if (m_isUninstall) {
        QString installationDir =  m_Info.InstallPath;
        QString uninstaller = m_Info.InstallPath + "\\uninstaller.exe";
        QString tmpPath = QStandardPaths::standardLocations(QStandardPaths::TempLocation).first();
        QString tmpUninstaller = tmpPath + "\\deepin-unistaller.exe";
        QFile::copy(uninstaller, tmpUninstaller);
        qDebug() << uninstaller;
        qDebug() << tmpUninstaller;
        QProcess caller;
        QString cmdline = tmpUninstaller + QString(" -u -t \"%1\"").arg(installationDir);
        qDebug() << "start delay remove " << cmdline;
        caller.startDetached(cmdline);
    }
    return 0;
}

int WindowsBackend::CreateUninstaller()
{
    int ret = Failed;
    FunctionLoger<int> log("CreateUninstaller", ret);
    this->SetAction("CreateUninstaller");
    this->Increment(2);

    QString uninstaller = QString("uninstaller.exe");
    QString uninstallerPath = m_Info.InstallPath + "/" + uninstaller;
    bool result = QFile::copy(QApplication::applicationFilePath(), uninstallerPath);
    qDebug() << "Copy app to" << uninstallerPath << " Result" << result;

    QSettings settings(RegistryKey, QSettings::NativeFormat);
    settings.setValue("UninstallString", QDir::toNativeSeparators(uninstallerPath));
    settings.setValue("InstallationDir", QDir::toNativeSeparators(m_Info.InstallPath));
    settings.setValue("InstallationDev", QDir::toNativeSeparators(m_Info.TargetDev));
    settings.setValue("ReleaseInfo",  m_Info.ReleaseInfo);
    settings.setValue("DisplayName",  m_Info.DistroName);
    settings.setValue("DisplayIcon", QDir::toNativeSeparators(m_Info.InstallPath + "\\deepin.ico"));
    settings.setValue("DisplayVersion", "0.10");
    settings.setValue("Publisher", m_Info.DistroName);
    settings.setValue("URLInfoAbout", "www.linuxdeepin.com");
    settings.setValue("HelpLink", "www.linuxdeepin.com");

    this->Increment(3);
    return ret = Success;
}

bool WindowsBackend::HasInstalled()
{
    qDebug() << "Begin HasInstalled";

    QSettings settings(RegistryKey, QSettings::NativeFormat);
    QStringList list = settings.childKeys();
    if (list.contains(UninstallString)) {
        return true;
    }
    return false;
}

int WindowsBackend::UninstallApp()
{
    int ret = Failed;
    FunctionLoger<int> log("UninstallApp", ret);

    //Recover Bootloaer
    switch (WindowsBootLoaderType()) {
    case BCD_BIOS:
        //Detect if uefi Install
        ret = this->UninstallBCD();
        break;
    case BCD_UEFI:
        ret = this->UninstallUEFI();
        break;
    case BOOT_INI:
        ret = this->UninstallBootIni();
        break;
    default:
        return ret = Failed;
    }

    QSettings settings(RegistryKey, QSettings::NativeFormat);
    QString installationDir = settings.value("InstallationDir").toString();
    Xapi::RmDir(installationDir);

    QString installationDev = settings.value("InstallationDev").toString();
    Xapi::RmFile(installationDev + "/wubildr");
    Xapi::RmFile(installationDev + "/wubildr.mbr");

    QSettings uninstallSettings(UninstallRegistryKey, QSettings::NativeFormat);
    uninstallSettings.remove(AppName);

    m_isUninstall = true;
    m_Info.InstallPath = installationDir;
    return ret = Success;
}

int WindowsBackend::FetchISO()
{
    int ret = Failed;
    FunctionLoger<int> log("FetchISO", ret);

    this->SetAction("FetchISO");

    QString curDir = QApplication::applicationDirPath();
    QString installISOPath = m_Info.InstallPath + "/install/install.iso";

    ProgressReporter<WindowsBackend> *pr = new ProgressReporter<WindowsBackend>(this);
    QString isofilepath = this->FetchImageFiles();
    qDebug() << "Find iso file :" << isofilepath;
    if (!isofilepath.isEmpty()) {
        m_Info.ImagePath = isofilepath;
        qDebug() << "ExCopy" << isofilepath << " to " << installISOPath;
        m_BasePersent = 30;
        m_BaseRange = 50;
        bool copyret = ExCopy(isofilepath, installISOPath, pr);
        return ret = copyret ? Success : Failed;
    }

    qDebug() << "Search iso file failed, try create iso image!";
    bool result = false;
    switch (DeviceType(curDir)) {
    case DRIVE_CDROM:
        qDebug() << "curDir: " << DRIVE_CDROM;
        m_BasePersent = 30;
        m_BaseRange = 50;
        result = DumpISO(curDir, installISOPath, pr);
        return ret = result ? Success : Failed;
    case DRIVE_NO_ROOT_DIR:
        //Make ISO FILE
        // TODO: md5sum.txt
        QString md5FileName = curDir + "/MD5SUM.TXT";
        QFile md5File(md5FileName);
        if (!md5File.exists()) {
            qDebug() << "MD5SUM.TXT not exists!";
            return ret = Failed;
        }

        //Verify md5
        if (!this->VerfiyMD5(curDir, md5FileName)) {
            qDebug() << "Verify md5 Failed";
            return ret = Failed;
        }

        //make iso
        QString mkisofs = Xapi::GetBlobs().Get(BlobAppMkisofs);
        QString cmdline = QString("-o %1 -V DeepinOS -r %2").arg(installISOPath).arg(curDir);
        Xapi::SynExec(mkisofs, cmdline);
        return ret = Success;
    }
    return ret = Failed;
}

int WindowsBackend::CreateVirtualDisks()
{
    int ret = Failed;
    FunctionLoger<int> log("CreateVirtualDisks", ret);

    this->SetAction("CreateVirtualDisks");
    this->Increment(1);

    QString rootDisk = m_Info.TargetDev + m_Info.RootFilePath;
    bool result = CreateVirtualFile(rootDisk, m_Info.RootSize);
    qDebug() << "Create " << rootDisk << result;
    this->Increment(2);

    QString swapDisk = m_Info.TargetDev + m_Info.SwapFilePath;
    result &= CreateVirtualFile(swapDisk, m_Info.SwapSize);
    qDebug() << "Create " << swapDisk << result;
    this->Increment(2);

    return ret = result ? Success : Failed;
}

int WindowsBackend::UninstallBCD()
{
    QString bcdedit = Xapi::SystemDirtory() + "\\bcdedit.exe";
    QSettings settings(RegistryKey, QSettings::NativeFormat);

    QString id = settings.value(BootloaderKey).toString();
    if (!id.isEmpty()) {
        QString cmd = QString("%1 /delete %2").arg(bcdedit).arg(id);
        Xapi::SynExec(cmd, "");
    }

    QString installationDev = settings.value("InstallationDev").toString();
    Xapi::RmFile(installationDev + "/wubildr");
    Xapi::RmFile(installationDev + "/wubildr.mbr");

    return Success;
}

bool WindowsBackend::VerfiyMD5(const QString &root, const QString md5FilePath)
{
    QFile md5File(md5FilePath);
    if (!md5File.open(QIODevice::ReadOnly)) {
        qDebug() << md5FilePath << " openfile";
        return false;
    }

    QString contents = md5File.readAll();
    QStringList checklist = contents.split("\n");
    foreach (QString checkitem, checklist) {
        QStringList md5path = checkitem.split("  ");
        QString md5 = md5path.first();
        QString path = root + "/" + md5path.last();
        QString fileMD5 = Xapi::GetFileMD5(path);
        if (md5 != fileMD5) {
            qDebug() << md5 << path << fileMD5;
            return false;
        }
    }
    return true;
}

int WindowsBackend::InstallBCD(QString &id)
{
    int ret = Failed;
    FunctionLoger<int> log("installBCD", ret);


    QString nativeTarget = m_Info.TargetDev.left(2);
    QString bootloaderDir =  m_Info.InstallPath + "/winboot";
    bootloaderDir.remove(nativeTarget);

    QString bootloaderPath = bootloaderDir + "/wubildr.mbr";
    QString bootloader = QDir::toNativeSeparators(bootloaderPath.remove(nativeTarget)).replace("\\\\", "\\");

    QString bcdedit = Xapi::SystemDirtory() + "\\bcdedit.exe";
    QString cmdline = QString(" /create /d %1 /application bootsector").arg(m_Info.DistroName);
    QString cmdret =  Xapi::SynExec(bcdedit, cmdline);
    id = cmdret.split("{").last().split("}").first();
    id = "{" + id + "}";
    cmdline = QString(" /set %1 device partition=%2").arg(id).arg(nativeTarget);
    Xapi::SynExec(bcdedit, cmdline);
    cmdline = QString(" /set %1 path %2").arg(id).arg(bootloader);
    Xapi::SynExec(bcdedit, cmdline);
    cmdline = QString(" /displayorder %1 /addlast").arg(id);
    Xapi::SynExec(bcdedit, cmdline);
    cmdline = QString(" /timeout  10");
    Xapi::SynExec(bcdedit, cmdline);
    cmdline = QString(" /bootsequence %1").arg(id);
    Xapi::SynExec(bcdedit, cmdline);

    return ret = Success;
}


int WindowsBackend::InstallUEFI(QString &id)
{
    QString espLetter = "B";
    QString app = "mountvol";
    QString cmd = QString("%1: /S").arg(espLetter);
    Xapi::SynExec(app, cmd);
    QDir efiroot(QString("%1:\\EFI\\").arg(espLetter));
    efiroot.rmdir("deepin_os");
    efiroot.mkdir("deepin_os");

    QString efiInstallPath = QString("%1:\\EFI\\deepin_os\\grubx64.efi").arg(espLetter);
    Xapi::CpFile(":/data/bootloader/grubx64.efi", efiInstallPath);

    QString efiBinPath = "\\EFI\\deepin_os\\grubx64.efi";
    qDebug() << "Insert UEFI boot option";
    QString bootoption = QString().fromStdWString(UEFI::InsertBootOption(
                             (espLetter + ":").toStdWString(),
                             L"Deepin on Windows",
                             efiBinPath.toStdWString()));

    qDebug() << "Umount efi partition";
    app = "mountvol";
    cmd = QString("%1: /d").arg(espLetter);
    Xapi::SynExec(app, cmd);

    qDebug() << "Umount set BootloaderKey" << bootoption;

    id = bootoption;
    return Success;
}

int WindowsBackend::UninstallUEFI()
{
    int ret = Failed;
    FunctionLoger<int> log("InstallBootloader", ret);

    QSettings settings(RegistryKey, QSettings::NativeFormat);
    QString bootxxxx = "Boot" + settings.value(BootloaderKey).toString();

    if (bootxxxx.isEmpty()) {
        return ret = Success;
    }

    qDebug() << "Remove uefi option: " << bootxxxx;
    UEFI::RemoveBootOption(bootxxxx.toStdWString());
    return ret = Success;
}

int WindowsBackend::InstallBootIni(QString &id)
{
    TCHAR path[MAX_PATH + 1];
    GetSystemDirectory(path, MAX_PATH);
    QString systemDev = QString().fromWCharArray(path).left(2).toUpper();
    QString bootloaderPath = systemDev + "\\" + "wubildr.mbr";
    QString bootloaderOption = QString("%1 = \"%2\"").arg(bootloaderPath).arg(m_Info.DistroName);

    Xapi::InsertFile(":/data/bootloader/wubildr.mbr", bootloaderPath);
    //read bootini and search [operating systems]
    QString bootiniPath = systemDev + "\\boot.ini";
    Xapi::SynExec("attrib", QString("-R -S -H %1").arg(bootiniPath));
    QFile bootini(bootiniPath);

    if (!bootini.open(QIODevice::ReadOnly)) {
        qDebug() << "Open Failed" << bootiniPath;
        return Failed;
    }

    QString bootcontent = bootini.readAll();
    bootini.close();

    qDebug() << "boot.ini" << bootcontent.length() << endl << bootcontent;
    bootcontent = bootcontent.remove("\r");
    qDebug() << "boot.ini" << bootcontent.length() << endl << bootcontent;

    if (bootcontent.isEmpty()) {
        qDebug() << "Read Failed anagin, abort" << bootiniPath;
    }

    if (!bootini.open(QIODevice::WriteOnly)) {
        qDebug() << "Open Failed" << bootiniPath;
        return Failed;
    }

    QStringList bootlines = bootcontent.split("\n");
    QStringList::iterator itor = bootlines.begin();
    while (itor != bootlines.end()) {
        if (itor->contains("[operating systems]")) {
            ++itor;
            while (itor != bootlines.end()) {
                if (!itor->contains("=")) {
                    bootlines.insert(itor, bootloaderOption);
                    break;
                }
                ++itor;
            }
            if (itor == bootlines.end()) {
                bootlines.insert(itor, bootloaderOption);
            }
            break;
        }
        ++itor;
    }

    itor = bootlines.begin();
    while (itor != bootlines.end()) {
        QString newline = (*itor).remove("\r") + "\r\n";
        if (newline.contains("timeout=")) {
            int time = newline.remove("timeout=").toInt();
            if (0 == time) {
                time = 5;
            }
            newline = QString("timeout=%1\r\n").arg(time);
        }
        ++itor;
        if (itor == bootlines.end()) {
            newline.remove("\r\n");
        }
        bootini.write(newline.toLatin1());
    }
    bootini.close();

    id = bootloaderOption;

    Xapi::SynExec("attrib", QString("+R +S +H %1").arg(bootiniPath));
    return Success;
}

int WindowsBackend::UninstallBootIni()
{
    QSettings settings(RegistryKey, QSettings::NativeFormat);

    QString bootentry = settings.value(BootloaderKey).toString();
    qDebug() << "Uinstall bootentry" << bootentry;
    if (!bootentry.contains("wubildr.mbr")) {
        return Failed;
    }

    TCHAR path[MAX_PATH + 1];
    GetSystemDirectory(path, MAX_PATH);
    QString systemDev = QString().fromWCharArray(path).left(2).toUpper();
    QString bootiniPath = systemDev + "\\boot.ini";
    Xapi::SynExec("attrib", QString("-R -S -H %1").arg(bootiniPath));

    QFile bootini(bootiniPath);
    if (!bootini.open(QIODevice::ReadOnly)) {
        qDebug() << "Open Failed" << bootiniPath;
        return Failed;
    }

    QString bootcontent = bootini.readAll();
    bootini.close();
    qDebug() << "boot.ini" << bootcontent.length() << endl << bootcontent;
    bootcontent = bootcontent.remove("\r");
    qDebug() << "boot.ini" << bootcontent.length() << endl << bootcontent;
    if (bootcontent.isEmpty()) {
        qDebug() << "Read Failed anagin, abort" << bootiniPath;
    }

    if (!bootini.open(QIODevice::WriteOnly)) {
        qDebug() << "Open Failed" << bootiniPath;
        return Failed;
    }

    QStringList bootlines = bootcontent.split("\n");

    QStringList::iterator itor = bootlines.begin();
    while (itor != bootlines.end()) {
        if (itor->contains(bootentry)) {
            ++itor;
            continue;
        }
        QString newline = (*itor).remove("\r") + "\r\n";
        ++itor;
        if (itor == bootlines.end()) {
            newline.remove("\r\n");
        }
        qDebug() << "newline " << newline;
        qDebug() << "newline latin1 " << newline.toLatin1();
        bootini.write(newline.toLatin1());
    }

    bootini.close();
    Xapi::SynExec("attrib", QString("+R +S +H %1").arg(bootiniPath));
    return Success;
}

int WindowsBackend::InstallBootloader()
{
    int ret = Failed;
    FunctionLoger<int> log("InstallBootloader", ret);

    this->SetAction("CreateConfig");

    QString nativeTarget = m_Info.TargetDev.left(2);
    QString bootloaderDir =  m_Info.InstallPath + "/winboot";
    qDebug() << nativeTarget << bootloaderDir;
    Xapi::CpFile(":/data/bootloader/wubildr", bootloaderDir + "/wubildr");
    Xapi::CpFile(":/data/bootloader/wubildr.mbr", bootloaderDir + "/wubildr.mbr");
    Xapi::CpFile(":/data/bootloader/wubildr.cfg", bootloaderDir + "/wubildr.cfg");
    Xapi::CpFile(":/data/bootloader/wubildr.tar", bootloaderDir + "/wubildr.tar");
    Xapi::CpFile(":/data/bootloader/wubildr-bootstrap.cfg", bootloaderDir + "/wubildr-bootstrap.cfg");
    Xapi::CpFile(":/data/bootloader/wubildr", nativeTarget + "/wubildr");
    Xapi::CpFile(":/data/bootloader/wubildr.mbr", nativeTarget + "/wubildr.mbr");

    this->Increment(1);
    //Test for win7 first
    QString bootid;
    switch (WindowsBootLoaderType()) {
    case BCD_BIOS:
        //Detect if uefi Install
        ret = this->InstallBCD(bootid);
        break;
    case BCD_UEFI:
        ret = this->InstallUEFI(bootid);
        break;
    case BOOT_INI:
        ret = this->InstallBootIni(bootid);
        break;
    default:
        ret = Failed;
        break;
    }

    QSettings settings(RegistryKey, QSettings::NativeFormat);
    settings.setValue(BootloaderKey, bootid);
    settings.setValue("ReleaseInfo",  m_Info.ReleaseInfo);

    this->Increment(2);
    return ret;
}

int WindowsBackend::InstallGrub()
{
    int ret = Failed;
    FunctionLoger<int> log("InstallGrub", ret);

    this->SetAction("InstallGrub");

    QFile grubTemplate(":/data/install/grub.install.cfg");
    if (!grubTemplate.open(QIODevice::ReadOnly)) {
        return ret = Failed;
    }
    QString content = grubTemplate.readAll();
    grubTemplate.close();

    QString iso_method = "iso-scan/filename";
    if (m_Info.BootMethod == "live") {
        iso_method = "findiso";
    }

    QMap<QString, QString> grubInfo;
//    grubInfo.insert("custom_installation_dir", m_Info.InstallPath);
    QString relativeImagePath = ToRelativePath(m_Info.InstallPath) + "/install/install.iso";
    grubInfo.insert("iso_path", relativeImagePath);
    grubInfo.insert("keyboard_variant", m_Info.KeyboardVariant);
    grubInfo.insert("keyboard_layout", m_Info.KeyboardLayout);
    grubInfo.insert("locale", m_Info.Locale);
    grubInfo.insert("accessibility", m_Info.Accessibility);
    grubInfo.insert("kernel", ToRelativePath(m_Info.KernelPath));
    grubInfo.insert("initrd", ToRelativePath(m_Info.InitrdPath));
    grubInfo.insert("rootflags", m_Info.RootFlags);
    grubInfo.insert("title1", "Completing the Deepin installation.");
    grubInfo.insert("title2", "For more installation boot options, press `ESC' now...");
    grubInfo.insert("normal_mode_title", "Normal mode");
    grubInfo.insert("safe_graphic_mode_title", "Safe graphic mode");
    grubInfo.insert("acpi_workarounds_title", "ACPI workarounds");
    grubInfo.insert("verbose_mode_title", "Verbose mode");
    grubInfo.insert("demo_mode_title",  "Demo mode");
    grubInfo.insert("intall_path",  m_Info.InstallPrefix);
    grubInfo.insert("iso_method", iso_method);
    grubInfo.insert("boot_method", m_Info.BootMethod);

    QMap<QString, QString>::iterator iter;
    iter = grubInfo.begin();
    while (iter != grubInfo.end()) {
        content.replace("$(" + iter.key() + ")", iter.value());
        ++iter;
    }

    QFile newGrub(m_Info.InstallPath + "/install/boot/grub/grub.cfg");
    if (!newGrub.open(QIODevice::WriteOnly)) {
        return ret = Failed;
    }
    if (content.length() != newGrub.write(content.toLatin1())) {
        return ret = Failed;
    }
    newGrub.close();
    this->Increment(1);
    return ret = Success;
}


int WindowsBackend::CreateConfig()
{
    int ret = Failed;
    FunctionLoger<int> log("CreateConfig", ret);

    this->SetAction("CreateConfig");

    QFile configTemplate(":/data/deepin-installer.conf");
    configTemplate.open(QIODevice::ReadOnly);
    QString content = configTemplate.readAll();
    configTemplate.close();

    QString loopdev = "/dev/loop5";
    QMap<QString, QString> configInfo;
    configInfo.insert("bootloader", loopdev);
    configInfo.insert("uefi", "false");
    configInfo.insert("simple_mode", "true");
    configInfo.insert("keybordvariant", m_Info.KeyboardVariant);
    configInfo.insert("keybordlayout", m_Info.KeyboardLayout);
    configInfo.insert("locale", m_Info.Locale);
    configInfo.insert("usename", m_Info.Username);
    configInfo.insert("password", m_Info.Password);
    configInfo.insert("hostname", m_Info.Hostname);
    configInfo.insert("timezone", m_Info.Timezone);
    configInfo.insert("root_partition", loopdev);
    configInfo.insert("root_disk", loopdev);
    configInfo.insert("root_file", m_Info.RootFilePath);
    configInfo.insert("swap_file", m_Info.SwapFilePath);

    QMap<QString, QString>::iterator iter;
    iter = configInfo.begin();
    while (iter != configInfo.end()) {
        content.replace("$(" + iter.key() + ")", iter.value());
        ++iter;
    }

    QFile newConfig(m_Info.InstallPath + "/install/deepin-installer.conf");
    if (!newConfig.open(QIODevice::WriteOnly)) {
        return ret = Failed;
    }

    content = content.remove("\r");

    if (content.length() != newConfig.write(content.toLatin1())) {
        return ret = Failed;
    }
    newConfig.close();

    this->Increment(1);
    return ret = Success;
}
