#pragma once

#include <QString>
class QFile;
struct _PARTITION_INFORMATION_EX;

#include "blobs.h"

namespace Xapi {

QString RandString(const QString &str = "");

QString TmpFilePath(const QString &filename = "");

QString InsertFileData(const QString &name, const QByteArray &data = "");

QString InsertTmpFile(const QString &fileurl);

QString InsertTmpFileList(const QString &subdir, const QStringList &fileurlList);

QString InsertFile(const QString &fileurl, const QString &fullpath);

QString SynExec(const QString &exec, const QString &param, const QString &execPipeIn="");

void RmFile(QFile &file);

void RmFile(const QString &filename);

bool CpFile(const QString &srcName, const QString &desName);

void MoveDir(const QString &oldName, const QString &newName);

bool RmDir(const QString &dirpath);

bool ImageDump(const QString& device, const QString& imagePath);

QString GetPartitionGUID(QString targetDev);

bool GetPartitionEx(QString targetDev, void* partInfo);

bool GetPartition(QString targetDev, void* partInfo);

QString Username();

QString Timezone();

QString Hostname();

QString KeyboardLayoutName();

QString KeyboardLayoutVariant();

QString Locale();

Blobs& GetBlobs();

int DumpISO(QString targetDev, const QString &imagePath);

QString GetFileMD5(const QString &filePath);

void Reboot();

enum Arch{
    AMD64,
    X86,
    UnknownArch,
};

Arch CpuArch ();


QString SystemDirtory() ;

}
