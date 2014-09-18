#include "xsys.h"

#include <QDebug>
#include <QFile>

#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "Ole32.lib")

namespace Xapi {

BOOL GetPartitionByHandle(HANDLE handle, PARTITION_INFORMATION_EX& partInfo){
   DWORD count;
   qDebug()<<handle;
   BOOL result = DeviceIoControl(
     (HANDLE) handle,                 // handle to a partition
     IOCTL_DISK_GET_PARTITION_INFO_EX, // dwIoControlCode
     NULL,                             // lpInBuffer
     0,                                // nInBufferSize
     (LPVOID) &partInfo,             // output buffer
     (DWORD) sizeof(partInfo),           // size of output buffer
     (LPDWORD) &count,        // number of bytes returned
     (LPOVERLAPPED) NULL       // OVERLAPPED structure
   );
   return result;
}

QString GetGUIDByHandle(HANDLE handle){
    PARTITION_INFORMATION_EX partInfo;
    if (!GetPartitionByHandle(handle, partInfo)) {
        qWarning()<<"IOCTL_DISK_GET_PARTITION_INFO_EX Failed!";
        return "";
    }
    const size_t MaxGUIDLen = 49;
    char format[MaxGUIDLen+1];
    GUID &guid = partInfo.Gpt.PartitionId;
    sprintf_s(format, MaxGUIDLen, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return QString(format).toLatin1();
}

bool GetPartition(QString targetDev, void* partInfo){
    if (NULL == partInfo) {
        return false;
    }
    PARTITION_INFORMATION_EX &part = *(PARTITION_INFORMATION_EX*)partInfo;
    QString driverName = "\\\\.\\" + targetDev.remove('\\');
    HANDLE handle = CreateFile(driverName.toStdWString().c_str(), GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ | FILE_SHARE_WRITE,
              NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        qWarning()<<"Open Dev Failed: "<<driverName<<endl;
        return -1;
    }
    return GetPartitionByHandle(handle, part);
}

QString GetPartitionGUID(QString targetDev) {
    QString driverName = "\\\\.\\" + targetDev.remove('\\');
    WCHAR wdriverName[1024] = {0};
    driverName.toWCharArray(wdriverName);
    HANDLE handle = CreateFile(driverName.toStdWString().c_str(), GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ | FILE_SHARE_WRITE,
              NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        qWarning()<<"Open Dev Failed: "<<driverName<<endl;
        return -1;
    }
    QString guid = GetGUIDByHandle(handle);
    CloseHandle(handle);
    return guid;
}


int DumpISO(QString targetDev, const QString &imagePath) {
    QString driverName = "\\\\.\\" + targetDev.remove('\\').remove("/");
    WCHAR wdriverName[1024] = {0};
    driverName.toWCharArray(wdriverName);
    HANDLE handle = CreateFile(driverName.toStdWString().c_str(), GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ | FILE_SHARE_WRITE,
              NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        qWarning()<<"Open Dev Failed: "<<driverName<<endl;
        return -1;
    }
    char * buffer = new char[1024*1024];
    QFile out(imagePath);
    out.open(QIODevice::WriteOnly);
    DWORD read = 0;
    bool ret = false;
    do {
        ret = ReadFile(handle, buffer, 1024*1024, &read, NULL);
        qWarning()<<read;
        out.write(buffer, read);
    }while(ret && (0!=read));
    return 0;
}


}
