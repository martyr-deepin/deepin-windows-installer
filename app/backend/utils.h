#pragma once

#include "progressreporter.h"

#include <QPair>
#include <QString>
#include <QList>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>

#include <Windows.h>

namespace DeepinInstaller{

bool IsValidUsernameFirstChar(const QString& username, QString& error);

bool IsValidUsernameChar(const QString& username, QString& error);

bool IsValidUsername(const QString& username, QString& error);

QString ToDeepinUsername(const QString& username);

QString ToDeepinHostname(const QString& username);


enum PartitonStyle {
    GPTPartition,
    MBRPartition,
    RAWPartition,
    UnknowPartition,
};

struct DiskInfo {
    QString Name;
    quint64 FreeSpace; //GB
    PartitonStyle Style;
};

QList<DiskInfo> GetLocalDiskList(quint64 minSizeInGb = 10,
                                                 const QString &type="NTFS",
                                                 const QString &table="MBR,GPT,RAW");

template <class T>
bool ExCopy(const QString &src, const QString &dest, ProgressReporter<T> *pr){
    QFile srcfile(src);
    QFile destfile(dest);

    if (NULL == pr){
        return false;
    }

    if (!srcfile.open (QIODevice::ReadOnly)) {
        return false;
    }

    if (!destfile.open (QIODevice::WriteOnly)){
        return false;
    }

    QFileInfo srcinfo(src);
    quint64 totalSize = srcinfo.size ();
    quint64 copySize = 0;
    quint64 BufferSize = 8 * 1024 * 1024;

    do{
        QByteArray buffer = srcfile.read (BufferSize);
        destfile.write (buffer);
        copySize += buffer.size ();
        pr->Report (totalSize, copySize);
    }while(copySize<totalSize);

    srcfile.close ();
    destfile.close ();

    return true;
}

template <class T>
QString GetFileMD5(const QString &filePath, ProgressReporter<T> *pr) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile in(filePath);
    QFileInfo fileInfo(filePath);
    quint64 fileSize = fileInfo.size();
    quint64 totalSize = fileSize;

    const quint64 bufferSize = 1024*16;
    if (in.open(QIODevice::ReadOnly)) {
        char buf[bufferSize];
        quint64 bytesRead = 0;

        int readSize = qMin(fileSize, bufferSize);
        while (readSize > 0 && (bytesRead = in.read(buf, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buf, bytesRead);
            readSize = qMin(fileSize, bufferSize);
            pr->Report (totalSize, totalSize-fileSize);
        }

        in.close();
        return hash.result().toHex();
    }
    return "";
}

template <class T>
bool DumpISO(QString targetDev, const QString &imagePath, ProgressReporter<T> *pr) {
    QString driverName = "\\\\.\\" + targetDev.remove('\\').remove("/");
    WCHAR wdriverName[1024] = {0};
    driverName.toWCharArray(wdriverName);
    HANDLE handle = CreateFile(driverName.toStdWString().c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               OPEN_EXISTING,
                               0,
                               NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        qWarning()<<"Open Dev Failed: "<<driverName;
        return false;
    }

    quint64 totalSize = GetFileSize(handle, NULL);
    quint64 copySize = 0;

    char * buffer = new char[1024*1024];
    QFile out(imagePath);
    if (!out.open(QIODevice::WriteOnly)){
        qWarning()<<"Open imagePath Failed: "<<imagePath;
        return false;
    }

    DWORD curReadSize = 0;
    bool ret = false;
    do {
        ret = ReadFile(handle, buffer, 1024*1024, &curReadSize, NULL);
        out.write(buffer, curReadSize);
        copySize += curReadSize;
        pr->Report (totalSize, copySize);
    }while(ret && (0!=curReadSize));
    return ret;
}

}
