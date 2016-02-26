#include "utils.h"

#include <string>

#include <QString>
#include <QPair>
#include <QList>
#include <QFileInfoList>
#include <QDir>

#include <cctype>

#include <Windows.h>

#include <xsys.h>

using namespace std;

static bool isLegal(char val) {
    switch (val){
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
    case '_':
        return true;
    }
    return false;
}

static bool isLowcase(char val) {
    switch (val){
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
        return true;
    }
    return false;
}

static bool isUpcase(char val) {
    switch (val){
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
        return true;
    }
    return false;
}

static bool isHostnameChar(char val) {
    if (isalnum(val)) {
        return true;
    }

    switch (val){
    case '-':
    case '.':
        return true;
    }
    return false;
}

static QString s_ReserveUsername[] = {
    "root",
    "daemon",
    "bin",
    "sys",
    "sync",
    "games",
    "man",
    "lp",
    "mail",
    "news",
    "uucp",
    "proxy",
    "www-data",
    "backup",
    "list",
    "irc",
    "gnats",
    "nobody",
    "adm",
    "tty",
    "disk",
    "kmem",
    "dialout",
    "fax",
    "voice",
    "cdrom",
    "floppy",
    "tape",
    "sudo",
    "audio",
    "dip",
    "operator",
    "src",
    "shadow",
    "utmp",
    "video",
    "sasl",
    "plugdev",
    "staff",
    "users",
    "nogroup",
    "netplan",
    "ftn",
    "mysql",
    "tac-plus",
    "alias",
    "qmail",
    "qmaild",
    "qmails",
    "qmailr",
    "qmailq",
    "qmaill",
    "qmailp",
    "asterisk",
    "vpopmail",
    "vchkpw",
    "Debian-exim",
    "admin",
    "bind",
    "crontab",
    "cupsys",
    "dcc",
    "dhcp",
    "dictd",
    "dovecot",
    "fetchmail",
    "firebird",
    "ftp",
    "fuse",
    "gdm",
    "haldaemon",
    "hplilp",
    "identd",
    "jwhois",
    "klog",
    "lpadmin",
    "messagebus",
    "mythtv",
    "netdev",
    "powerdev",
    "radvd",
    "saned",
    "sbuild",
    "scanner",
    "slocate",
    "ssh",
    "sshd",
    "ssl-cert",
    "sslwrap",
    "statd",
    "syslog",
    "telnetd",
    "tftpd",
    "lightdm",
};

#include <QDebug>

namespace DeepinInstaller {

bool IsValidUsernameFirstChar(const QString& username, QString& error) {
    for(int i = 0; i < username.length(); ++i) {
        if (!isLowcase(username.at(i).toLatin1())) {
            error = QObject::tr("The first character must be in lower case.");
            return false;
        }
    }
    return true;
}


bool IsValidUsernameChar(const QString& username, QString& error) {
    for(int i = 0; i < username.length(); ++i) {
        if (!isLegal(username.at(i).toLatin1())) {
            error = QObject::tr("Username must comprise a~z, 0~9, - or _. ");
            return false;
        }
    }
    return true;
}

bool IsValidUsername(const QString& username, QString& error){
    if (username.length() ==0) {
        error = QObject::tr("Username can not be empty. ");
        return false;
    }

    for(int i = 0; i < username.length(); ++i) {
        if (!isLegal(username.at(i).toLatin1())) {
            error = QObject::tr("Username must comprise a~z, 0~9, - or _. ");
            return false;
        }
    }

    if (!isLowcase(username.at(0).toLatin1())) {
        error = QObject::tr("The first character must be in lower case.");
        return false;
    }

    size_t n = sizeof(s_ReserveUsername)/sizeof(s_ReserveUsername[0]);
    for (size_t i =0; i < n; ++i) {
        if(0 == username.compare(s_ReserveUsername[i])) {
            error = QString(QObject::tr("The username has been used by system."));
            return false;
        }
    }

    return true;
}

QString ToDeepinUsername(const QString& username) {
    string stand = username.toLower().toStdString();
    string newstand = "";
    string::iterator itor = stand.begin();
    for (; itor!= stand.end(); ++itor) {
        if (isLegal(*itor)) {
            newstand += *itor;
        }
    }
    //first letter
    while (newstand.begin() != newstand.end()) {
        if (isLowcase(*newstand.begin())) {
            break;
        }
        newstand.erase(newstand.begin());
    }

    if (newstand.length()> 0 ) {
        return QString().fromStdString(newstand);
    }
    return "deepin";
}

QString ToDeepinHostname(const QString& hostname) {
    string stand = hostname.toStdString();
    string newstand = "";
    string::iterator itor = stand.begin();
    for (; itor!= stand.end(); ++itor) {
        if (isHostnameChar(*itor)) {
            newstand += *itor;
        }
    }
    //first letter
    while (newstand.begin() != newstand.end()) {
        if (isalpha(*newstand.begin())) {
            break;
        }
        newstand.erase(newstand.begin());
    }

    if (newstand.length()>64) {
        return QString().fromStdString(newstand).left(32);
    }
    if (newstand.length()>=1) {
        return QString().fromStdString(newstand);
    }
    return "deepin-pc";
}

QList<DiskInfo> GetLocalDiskList(quint64 minSizeInGb,
                                                 const QString &type,
                                                 const QString &table){
    QList<DiskInfo> disklist;
    QFileInfoList devlist = QDir::drives();
    for (int i = 0; i < devlist.size(); ++i)
    {
        QString devname = QDir::toNativeSeparators(devlist.at(i).path().toUpper()).remove("\\");
        if (GetDriveType(LPWSTR(devname.utf16())) == DRIVE_FIXED)
        {
            ULARGE_INTEGER FreeAv,TotalBytes,FreeBytes;
            if  (!GetDiskFreeSpaceEx(LPWSTR(devname.utf16()),&FreeAv,&TotalBytes,&FreeBytes)) {
                continue;
            }
            if ((FreeBytes.QuadPart/(1024*1024*1024)) < minSizeInGb) {
                qDebug()<<"Skip Disk"<<devname<<"Space not enough"<<FreeBytes.QuadPart/(1024*1024*1024);
                continue;
            }

            PARTITION_INFORMATION_EX piex;
            Xapi::GetPartitionEx (devname, &piex);

            QString format = "Unknow";
            PartitonStyle style;
            switch (piex.PartitionStyle) {
            case PARTITION_STYLE_MBR:
                style = MBRPartition;
                if (!table.toUpper ().contains ("MBR")) {
                    qDebug()<<"Skip Disk"<<devname<<"MBR";
                    continue;
                }

                switch (piex.Mbr.PartitionType) {
                case PARTITION_FAT32:
                case PARTITION_FAT_16:
                    format = "FAT";
                    break;
                case 0x4A:
                case PARTITION_IFS:
                    format = "NTFS";
                    break;
                default:
                    break;

                }

                if (!type.toUpper ().contains (format.toUpper ())) {
                    qDebug()<<"Skip Disk"<<devname<<format<<"Not a NTFS Disk";
                    continue;
                }

                break;
            case PARTITION_STYLE_GPT:
                style = GPTPartition;
                if (!table.toUpper ().contains ("GPT")) {
                     qDebug()<<"Skip Disk"<<devname<<"GPT";
                     continue;
                }

                GUID PARTITION_BASIC_DATA_GUID;
                PARTITION_BASIC_DATA_GUID.Data1 = 0xEBD0A0A2L;
                PARTITION_BASIC_DATA_GUID.Data2 = 0xB9E5;
                PARTITION_BASIC_DATA_GUID.Data3 = 0x4433;
                PARTITION_BASIC_DATA_GUID.Data4[0] = 0x87;
                PARTITION_BASIC_DATA_GUID.Data4[1] = 0xC0;
                PARTITION_BASIC_DATA_GUID.Data4[2] = 0x68;
                PARTITION_BASIC_DATA_GUID.Data4[3] = 0xB6;
                PARTITION_BASIC_DATA_GUID.Data4[4] = 0xB7;
                PARTITION_BASIC_DATA_GUID.Data4[5] = 0x26;
                PARTITION_BASIC_DATA_GUID.Data4[6] = 0x99;
                PARTITION_BASIC_DATA_GUID.Data4[7] = 0xC7;

                if (PARTITION_BASIC_DATA_GUID != piex.Gpt.PartitionType) {
                    qDebug()<<"Skip Disk"<<devname<<"Not a NTFS Disk";
                    continue;
                }
                break;
            case PARTITION_STYLE_RAW:
                style = RAWPartition;
                if (!table.toUpper ().contains ("RAW")) {
                     continue;
                }
                break;
            default:
                style = UnknowPartition;
                qDebug()<<"Disk Type"<<devname<<"Unknow";
                continue;
            }

            DiskInfo diskinfo;
            diskinfo.Name = devname;
            diskinfo.Style = style;
            diskinfo.FreeSpace = FreeBytes.QuadPart/(1024*1024*1024);
            QString result = Xapi::SynExec( "cmd",  QString("/C \"chcp 437 & %1\\manage-bde -status  %2\" ").arg(Xapi::SystemDirtory()).arg(devname));
            QStringList list = result.split("\r").filter("BitLocker Version");
            diskinfo.Encrypt = list.size() ? !list.first().contains("None") : false;
            qDebug()<<devname<<"System Type"<<diskinfo.Encrypt;
            disklist.push_back(diskinfo);
        }
    }
    return disklist;
}

}
