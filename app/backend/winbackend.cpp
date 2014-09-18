#include "winbackend.h"

#include <xsys.h>

#include <fireware.h>

#include <windows.h>
#include <VersionHelpers.h>

#include <QFile>
#include <QDir>
#include <QSettings>
#include <QApplication>
#include <QDebug>

using namespace DeepinInstaller;

QString  UninstallRegistryKey = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
QString AppName = "DeepinWindowsInstaller";

enum WindowsVersion{
    WinUnknow,
    Win2000,
    WinXP,
    WinVista,
    WinSeven,
    WinEight,
};

// TODO: use VerifyVersionInfo
#pragma warning (disable : 4996)

int GetWindowsVersion() {
    OSVERSIONINFOEX os;
    os.dwOSVersionInfoSize=sizeof(os);
    if(!GetVersionEx((OSVERSIONINFO *)&os))
    {
          return WinUnknow;
    }

    switch(os.dwMajorVersion) {
    case 5:
        switch(os.dwMinorVersion){
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
        switch(os.dwMinorVersion) {
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
            switch(os.wProductType) {
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

enum BootloaderType{
    BCD_UEFI,
    BCD_BIOS,
    BOOT_INI,
    BL_UNKNOW,
};

BootloaderType BCDType () {
    QString cmd = QString("bcdedit.exe /enum");
    QString ret =  Xapi::SynExec(cmd, "");

    QStringList bootEntryList =ret.split("-------------------").filter("bootmgr");
    for (auto entry: bootEntryList) {
        if (entry.contains("bootmgfw.efi")){
            return BCD_UEFI;
        }
    }
    return BCD_BIOS;
}

BootloaderType WindowsBootLoaderType() {
    //Test for win7 first
    switch (GetWindowsVersion()) {
    case WinSeven:
    case WinEight:
        //Detect if uefi Install
        return BCDType();
    default:
        return BL_UNKNOW;
    }
}

int ESPVolnameIndex() {
    QString cmd = QString("bcdedit.exe /enum");
    QString ret =  Xapi::SynExec(cmd, "");

    QString bootmgr = ret.split("-------").filter("bootmgr").first();
    QString device = bootmgr.split("\n").filter("partition=\\Device").first();
    device = device.split("=").last();
    device = device.remove("\\Device\\HarddiskVolume");
    return device.toInt();
}

int DeviceType(const QString &target) {
    return GetDriveType(target.toStdWString().c_str());
}

QString ToRelativePath(const QString& windowsPath) {
    auto rpath = windowsPath;
    rpath.replace("\\", "/");
    if ((rpath.length() > 1) && (':' == rpath[1])) {
        rpath = rpath.right(rpath.length() - 2);
    }

    if ((rpath.length() > 1) && ("/" == rpath.right(1))) {
        rpath = rpath.left(rpath.length() - 1);
    }
    return rpath;
}

bool CreateVirtualFile(const QString& filePath, quint64 size) {
    auto handle = CreateFile(
                filePath.toStdWString().c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    quint64 fileBytes = size * 1024 * 1024;
    LARGE_INTEGER filePos;
    filePos.QuadPart = fileBytes;

    if (!SetFilePointerEx(handle, filePos, 0, FILE_BEGIN)) {
        return false;
    }

    if (!SetEndOfFile(handle)) {
        return false;
    }

    SetFileValidData(handle, filePos.QuadPart);

    filePos.QuadPart = 0;
    if (!SetFilePointerEx(handle, filePos, 0, FILE_BEGIN)) {
        return false;
    }

    //zero File
    quint64 bufSize = 1024*1024;
    quint8 *writeBuf = new quint8[bufSize];
    quint64 writeBufSize = bufSize;
    memset(writeBuf, NULL, writeBufSize);

    quint64 byteCleared = 0;
    quint64 bytesWrite = writeBufSize;
    DWORD bytesWrited = 0;
    while(byteCleared < 1000000) {
        bytesWrite = writeBufSize;
        if (bytesWrite > (fileBytes - byteCleared)) {
            bytesWrite = fileBytes - byteCleared;
        }

        bool ret=WriteFile(handle, writeBuf, bytesWrite, &bytesWrited, NULL);
        if (!ret || (0 == bytesWrited)) {
            return false;
        }
        byteCleared += bytesWrited;
    }

    return true;
}

static QString EasyUEFIApp = "EasyUEFI.exe";

static QString BlobAppMkisofs = "mkisofs.exe";

int UnpackBootXXXX(const QString &bootxxxx) ;

WindowsBackend::WindowsBackend(const QString &installTarget,
               const QString &isoPath,
               const QString &username,
               const QString &password):
    Backend(installTarget, isoPath, username, password){
//Debug: "\EFI\deepin\boot\grubx64.efi"
    UEFI::InsertBootOption(L"B:\\"   ,
                           L"Deepin",
                           L"\\Boot\\deepin\\boot\\grubefi64.efi");
    UnpackBootXXXX("Boot0026");
    UnpackBootXXXX("Boot0027");

    QStringList easyUEFIList;
    easyUEFIList.append(":/blobs/easyuefi/EasyUEFI.exe");
    easyUEFIList.append(":/blobs/easyuefi/EasyUEFI.ini");
    easyUEFIList.append(":/blobs/easyuefi/ExtractISO.dll");
    easyUEFIList.append(":/blobs/easyuefi/iconv.dll");
    easyUEFIList.append(":/blobs/easyuefi/libgcc_s_seh-1.dll");
    easyUEFIList.append(":/blobs/easyuefi/libintl-8.dll");
    easyUEFIList.append(":/blobs/easyuefi/libwim-9.dll");
    easyUEFIList.append(":/blobs/easyuefi/libxml2-2.dll");
    easyUEFIList.append(":/blobs/easyuefi/pthreadGC2.dll");

    //Xapi::GetBlobs().Install(EasyUEFIApp, easyUEFIList);
   // Xapi::GetBlobs().Install(BlobAppMkisofs, ":/blobs/tools/mkisofs.exe");
}

int WindowsBackend::CreateUninstaller(){
    QString uninstaller = QString("uinstall.exe");
    QString uninstallerPath = m_Info.InstallPath + "/" + uninstaller;
    QFile::copy(QApplication::applicationFilePath(), uninstallerPath);

    QString registryKey = UninstallRegistryKey + AppName;
    QSettings settings(registryKey, QSettings::NativeFormat);

    settings.setValue("UninstallString", uninstallerPath);
    settings.setValue("InstallationDir", m_Info.InstallPath);
    settings.setValue("InstallationDev", m_Info.TargetDev);
    settings.setValue("DisplayName",  m_Info.DistroName);
    settings.setValue("DisplayIcon", m_Info.InstallPath+"\\deepin.ico");
    settings.setValue("DisplayVersion", "0.99");
    settings.setValue("Publisher", m_Info.DistroName);
    settings.setValue("URLInfoAbout", "www.linuxdeepin.com");
    settings.setValue("HelpLink", "www.linuxdeepin.com");
    return Success;
}

bool WindowsBackend::HasInstalled() {
    QString bootloaderKey = "VistaBootDrive";
    QString registryKey = UninstallRegistryKey + AppName;

    QSettings settings(registryKey, QSettings::NativeFormat);
    QStringList list = settings.childKeys();
    if (list.contains(bootloaderKey)) {
        return true;
    }
    return false;
}

int WindowsBackend::Uninstall() {
    //Recover Bootloaer
    switch (GetWindowsVersion()) {
    case WinSeven:
        this->uninstallBCD();
        break;
    default:
        break;
    }

    QString registryKey = UninstallRegistryKey + AppName;
    QSettings settings(registryKey, QSettings::NativeFormat);
    QString installationDir = settings.value("InstallationDir").toString();
    Xapi::RmDir(installationDir);

    QSettings uninstallSettings(UninstallRegistryKey, QSettings::NativeFormat);
    uninstallSettings.remove(AppName);
    return Success;
}


int WindowsBackend::FetchISO() {
    qDebug()<<"WindowsBackend::FetchISO()";
    //Copy ISO File to (root)/install/install.iso
    QString installISOPath = m_Info.InstallPath+"/install/install.iso";
    QFile imageFile(m_Info.ImagePath);
    if (imageFile.exists()) {
        auto ret = QFile::copy(m_Info.ImagePath, installISOPath);
        return ret ? Success:Failed;
    }

    //make image
    auto curDir = QApplication::applicationDirPath();
    qDebug()<<"curDir: "<<curDir;

    switch(DeviceType(curDir)) {
    case DRIVE_CDROM:
        qDebug()<<"curDir: "<<DRIVE_CDROM;
        Xapi::DumpISO(curDir, installISOPath);
        return Success;
    case DRIVE_NO_ROOT_DIR:
        //Make ISO FILE
        // TODO: md5sum.txt
        QString md5FileName = curDir + "/MD5SUM.TXT";
        QFile md5File(md5FileName);
        if (!md5File.exists()) {
            qDebug()<<"MD5SUM.TXT not exists!";
            return Failed;
        }

        //Verify md5
        if (!this->verfiyMD5(curDir, md5FileName)) {
            qDebug()<<"Verify md5 Failed";
            return Failed;
        }

        //make iso
        QString mkisofs = Xapi::GetBlobs().Get(BlobAppMkisofs);
        QString cmdline = QString("-o %1 -V DeepinOS -r %2").arg(installISOPath).arg(curDir);
        Xapi::SynExec(mkisofs, cmdline);
        return Success;
    }
    return Failed;
}

int WindowsBackend::CreateVirtualDisks() {
    QString rootDisk = m_Info.InstallPath + "/disks/root.disk";
    auto ret = CreateVirtualFile(rootDisk, m_Info.RootSize-m_Info.SwapSize);

    QString swapDisk = m_Info.InstallPath + "/disks/swap.disk";
    ret &= CreateVirtualFile(swapDisk, m_Info.SwapSize);
    return ret;
}

int WindowsBackend::uninstallBCD() {
    QString registryKey = UninstallRegistryKey + AppName;
    QSettings settings(registryKey, QSettings::NativeFormat);

    QString id = settings.value("VistaBootDrive").toString();
    QString cmd = QString("bcdedit.exe /delete %1").arg(id);
    Xapi::SynExec(cmd, "");

    QString installationDev = settings.value("InstallationDev").toString();
    Xapi::RmFile(installationDev + "/wubildr");
    Xapi::RmFile(installationDev + "/wubildr.mbr");

    return Success;
}

bool WindowsBackend::verfiyMD5(const QString &root, const QString md5FilePath) {
    QFile md5File(md5FilePath);
    if (!md5File.open(QIODevice::ReadOnly)) {
        qDebug()<<md5FilePath<<" openfile";
        return false;
    }

    QString contents = md5File.readAll();
    QStringList checklist = contents.split("\n");
    for(QString& checkitem: checklist) {
        QStringList md5path = checkitem.split("  ");
        QString md5 = md5path.first();
        QString path = root + "/" + md5path.last();
        QString fileMD5 = Xapi::GetFileMD5(path);
        if (md5 != fileMD5) {
            qDebug()<<md5<<path<<fileMD5;
            return false;
        }
    }
    return true;
}

int WindowsBackend::installBCD() {
    QString bootloaderKey = "VistaBootDrive";
    QString registryKey = UninstallRegistryKey + AppName;

    QSettings settings(registryKey, QSettings::NativeFormat);
    QStringList list = settings.childKeys();
    if (list.contains(bootloaderKey)) {
        //TODO:
        //bcd areay install
        //return Success;
    }
    qDebug()<<"Install Bcd";

    QString nativeTarget = m_Info.TargetDev.left(2);
    QString bootloaderDir =  m_Info.InstallPath + "/winboot";
    qDebug()<<nativeTarget<<bootloaderDir;
    Xapi::CpFile(":/data/bootloader/wubildr", bootloaderDir + "/wubildr");
    Xapi::CpFile(":/data/bootloader/wubildr.mbr", bootloaderDir + "/wubildr.mbr");
    Xapi::CpFile(":/data/bootloader/wubildr.cfg", bootloaderDir + "/wubildr.cfg");
    Xapi::CpFile(":/data/bootloader/wubildr.tar", bootloaderDir + "/wubildr.tar");
    Xapi::CpFile(":/data/bootloader/wubildr-bootstrap.cfg", bootloaderDir + "/wubildr-bootstrap.cfg");

    Xapi::CpFile(":/data/bootloader/wubildr", nativeTarget + "/wubildr");
    Xapi::CpFile(":/data/bootloader/wubildr.mbr", nativeTarget + "/wubildr.mbr");

    bootloaderDir.remove(nativeTarget);
    QString bootloaderPath = bootloaderDir + "/wubildr.mbr";
    QString bootloader = QDir::toNativeSeparators(bootloaderPath.remove(nativeTarget)).replace("\\\\", "\\");

    //  command = [bcdedit, '/create', '/d', '%s' % self.info.distro.name, '/application', 'bootsector']
    QString cmd = QString("bcdedit.exe /create /d %1 /application bootsector").arg(m_Info.DistroName);
    QString ret =  Xapi::SynExec(cmd, "");
    QString id = ret.split("{").last().split("}").first();
    id = "{" + id + "}";
    qDebug()<<cmd;
    cmd = QString("bcdedit.exe /set %1 device partition=%2").arg(id).arg(nativeTarget);
    qDebug()<<cmd;
    Xapi::SynExec(cmd, "");
    cmd = QString("bcdedit.exe /set %1 path %2").arg(id).arg(bootloader);
    Xapi::SynExec(cmd, "");
    cmd = QString("bcdedit.exe /displayorder %1 /addlast").arg(id);
    Xapi::SynExec(cmd, "");
    cmd = QString("bcdedit.exe /timeout  10");
    Xapi::SynExec(cmd, "");
    cmd = QString("bcdedit.exe /bootsequence %1").arg(id);
    Xapi::SynExec(cmd, "");

    settings.setValue(bootloaderKey, id);
    return Success;
}


int WindowsBackend::installUEFI() {
    QString bootloaderKey = "VistaBootDrive";
    QString registryKey = UninstallRegistryKey + AppName;

    QSettings settings(registryKey, QSettings::NativeFormat);
    QStringList list = settings.childKeys();
    if (list.contains(bootloaderKey)) {
        //bcd areay install
        //return Success;
    }
    qDebug()<<"Install UEFI";

    QString nativeTarget = m_Info.TargetDev.left(2);
    QString bootloaderDir =  m_Info.InstallPath + "/winboot";
    qDebug()<<nativeTarget<<bootloaderDir;
    Xapi::CpFile(":/data/bootloader/wubildr", bootloaderDir + "/wubildr");
    Xapi::CpFile(":/data/bootloader/wubildr.mbr", bootloaderDir + "/wubildr.mbr");
    Xapi::CpFile(":/data/bootloader/wubildr.cfg", bootloaderDir + "/wubildr.cfg");
    Xapi::CpFile(":/data/bootloader/wubildr.tar", bootloaderDir + "/wubildr.tar");
    Xapi::CpFile(":/data/bootloader/wubildr-bootstrap.cfg", bootloaderDir + "/wubildr-bootstrap.cfg");

    Xapi::CpFile(":/data/bootloader/wubildr", nativeTarget + "/wubildr");
    Xapi::CpFile(":/data/bootloader/wubildr.mbr", nativeTarget + "/wubildr.mbr");

    QString espLetter = "B";
    QString easyuefi = Xapi::GetBlobs().Get(EasyUEFIApp);
    QString app = "mountvol";
    QString cmd = QString("%1: /S").arg(espLetter);
    Xapi::SynExec(app, cmd);
    QDir efiroot(QString("%1:\\EFI\\").arg(espLetter));
    efiroot.mkdir("Deepin");
    QString efiInstallPath = QString("%1:\\EFI\\Deepin\\grubx64.efi").arg(espLetter);
    Xapi::CpFile(":/data/bootloader/grubx64.efi", efiInstallPath);

    QString efiBinPath = "\\EFI\\Deepin\\grubx64.efi";
    QString guid = Xapi::GetPartitionGUID(QString("%1:").arg(espLetter));
    qDebug()<<guid;
    app = easyuefi;
    cmd = QString("--add-efi-guid --type 1 --name Deepin --guid %1 --file %2").arg(guid).arg(efiBinPath);
    Xapi::SynExec(app, cmd);

    app = "mountvol";
    cmd = QString("%1: /d").arg(espLetter);
    Xapi::SynExec(app, cmd);

    // TODO: Ge Boot####
    settings.setValue(bootloaderKey, guid);
    return Success;
}

int WindowsBackend::uninstallUEFI() {
    // TODO: Get Index of easyuefi
    return Failed;
}

int WindowsBackend::InstallBootloader() {
    //Test for win7 first
    switch (WindowsBootLoaderType()) {
    case BCD_BIOS:
        //Detect if uefi Install
        return  this->installBCD();
    case BCD_UEFI:
        return this->installUEFI();
    default:
        return Failed;
    }
}

int WindowsBackend::InstallGrub() {
    QFile grubTemplate(":/data/install/grub.install.cfg");
    grubTemplate.open(QIODevice::ReadOnly);
    QString content = grubTemplate.readAll();
    grubTemplate.close();

    QMap<QString, QString> grubInfo;
//    grubInfo.insert("custom_installation_dir", m_Info.InstallPath);
    QString relativeImagePath = ToRelativePath(m_Info.InstallPath) +"/install/install.iso";
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

    QMap<QString, QString>::iterator iter;
    iter = grubInfo.begin();
    while(iter != grubInfo.end()) {
        content.replace("$("+iter.key()+")", iter.value());
        ++iter;
    }

    QFile newGrub(m_Info.InstallPath + "/install/boot/grub/grub.cfg");
    newGrub.open(QIODevice::WriteOnly);
    newGrub.write(content.toLatin1());
    newGrub.close();
    return Failed;
}


int WindowsBackend::CreateConfig() {
    QFile configTemplate(":/data/deepin-installer.conf");
    configTemplate.open(QIODevice::ReadOnly);
    QString content = configTemplate.readAll();
    configTemplate.close();

    QMap<QString, QString> configInfo;
    configInfo.insert("bootloader", "/dev/loop2");
    configInfo.insert("uefi", "false");
    configInfo.insert("simple_mode", "true");
    configInfo.insert("keybordvariant", m_Info.KeyboardVariant);
    configInfo.insert("keybordlayout", m_Info.KeyboardLayout);
    configInfo.insert("locale", m_Info.Locale);
    configInfo.insert("usename", m_Info.Username);
    configInfo.insert("password", m_Info.Password);
    configInfo.insert("hostname", m_Info.Hostname);
    configInfo.insert("timezone", m_Info.Timezone);
    configInfo.insert("root_partition", "/dev/loop2");
    configInfo.insert("root_disk", "/dev/loop2");

    QMap<QString, QString>::iterator iter;
    iter = configInfo.begin();
    while(iter != configInfo.end()) {
        content.replace("$("+iter.key()+")", iter.value());
        ++iter;
    }

    QFile newConfig(m_Info.InstallPath + "/install/deepin-installer.conf");
    newConfig.open(QIODevice::WriteOnly);
    newConfig.write(content.toLatin1());
    newConfig.close();
    return Failed;
}
