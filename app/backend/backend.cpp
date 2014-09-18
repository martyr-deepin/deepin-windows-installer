#include "backend.h"

#include <xsys.h>

#include <QDir>
#include <QApplication>

using namespace DeepinInstaller;


static QString BlobAppSevenZ = "sevnz.exe";

Backend::Backend(const QString &installTarget,
                 const QString &isoPath,
                 const QString &username,
                 const QString &password) {
    m_Info.TargetDev = installTarget.left(2);
    m_Info.InstallPath = m_Info.TargetDev + "/" + "deepin";
    m_Info.KernelPath = m_Info.InstallPath + "/install/boot/vmlinuz.efi";
    m_Info.InitrdPath = m_Info.InstallPath + "/install/boot/initrd.lz";
    m_Info.ImagePath = isoPath;
    m_Info.DistroName = "Deepin";
    m_Info.RootSize = 8 * 1024;
    m_Info.SwapSize = 1 * 1024;
    m_Info.RootFlags = "rootflags=sync";
    m_Info.KeyboardVariant = Xapi::KeyboardLayoutVariant();
    m_Info.KeyboardLayout = Xapi::KeyboardLayoutName();
    m_Info.Locale =Xapi::Locale();

    m_Info.Username = username;
    m_Info.Password = password;
    m_Info.Hostname = "deepin";
    m_Info.Timezone = Xapi::Timezone();

    QStringList sevenzFileList;
    sevenzFileList.append(":/blobs/sevenz/sevnz.exe");
    sevenzFileList.append(":/blobs/sevenz/sevnz.dll");
   // Xapi::GetBlobs().Install(BlobAppSevenZ, sevenzFileList);
}

int Backend::CreateInstallDir() {
    QStringList installStructure;
    installStructure.append("deepin");
    installStructure.append("deepin/disks");
    installStructure.append("deepin/install");
    installStructure.append("deepin/install/boot");
    installStructure.append("deepin/install/boot/grub");
    installStructure.append("deepin/winboot");

    auto ret = true;
//    QDir target(m_Info.TargetDev);
//    target.mkdir("deepin/");
    QDir root(m_Info.TargetDev);
    for(auto v : installStructure) {
        ret &= root.mkdir(v);
    }
    return ret ? Success : Failed;
}

int Backend::ExtractISO() {
    // Extract Kernel and Initrd for iso
    QString imagePath = m_Info.InstallPath + "/install/install.iso";
    QString sevenz = Xapi::GetBlobs().Get(BlobAppSevenZ);

    QString cmdline = QString(" e ")
            + " -i!" + "casper/vmlinuz.efi "
            + " -o" + m_Info.InstallPath + "/install/boot "
            + imagePath;
    Xapi::SynExec(sevenz, cmdline);

    cmdline = QString(" e ")
            + " -i!" + "casper/initrd.lz "
            + " -o" + m_Info.InstallPath + "/install/boot "
            + imagePath;
    Xapi::SynExec(sevenz, cmdline);

    return Success;
}

/*
    int CreateUninstaller() = 0;

    int CreateVirtualFile() = 0;

    int CreatePressed() = 0;

    int InstallBootloader() = 0;

    int InstallGrub() = 0;
};*/

