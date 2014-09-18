#include "winbackend.h"

#include <xsys.h>

#include <windows.h>
#include <VersionHelpers.h>

#include <QFile>
#include <QDir>
#include <QSettings>
#include <QApplication>
#include <QDebug>

void RasiePrivilegesXXX(void)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
            &hToken)) {
                qDebug("Failed OpenProcessToken\r\n");
                return;
    }

    LookupPrivilegeValue(NULL, SE_SYSTEM_ENVIRONMENT_NAME,
        &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    DWORD len;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, &len);

    if (GetLastError() != ERROR_SUCCESS) {
        qDebug("Failed RasiePrivileges()\r\n");
        return;
    }
}
#pragma pack(1)

typedef struct {
  UINT8 Type;       ///< 0x01 Hardware Device Path
                    ///< 0x02 ACPI Device Path
                    ///< 0x03 Messaging Device Path
                    ///< 0x04 Media Device Path
                    ///< 0x05 BIOS Boot Specification Device Path
                    ///< 0x7F End of Hardware Device Path

  UINT8 SubType;    ///< Varies by Type
                    ///< 0xFF End Entire Device Path, or
                    ///< 0x01 End This Instance of a Device Path and start a new
                    ///< Device Path

  UINT8 Length[2];  ///< Specific Device Path data. Type and Sub-Type define
                    ///< type of data. Size of data is included in Length.

} EFI_DEVICE_PATH_PROTOCOL;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  UINT32                          PartitionNumber;
  UINT64                          PartitionStart;
  UINT64                          PartitionSize;
  UINT8                           Signature[16];
  UINT8                           MBRType;
  UINT8                           SignatureType;
} HARDDRIVE_DEVICE_PATH;

typedef struct _MediaHDD {
// 4 4 Describes the entry in a partition table, starting with entry 1.
//Partition number zero represents the entire device. Valid
//partition numbers for a MBR partition are [1, 4]. Valid partition
//numbers for a GPT partition are [1,
//NumberOfPartitionEntries].
    UINT32 PartitionNumber;
//8 8 Starting LBA of the partition on the hard drive
    UINT64 PartitionStart;
// 16 8 Size of the partition in units of Logical Blocks
    UINT64 PartitionSize;
////24 16 Signature unique to this partition:
//• If SignatureType is 0, this field has to be initialized with 16
//zeroes.
//• If SignatureType is 1, the MBR signature is stored in the
//first 4 bytes of this field. The other 12 bytes are initialized
//with zeroes.
//• If SignatureType is 2, this field contains a 16 byte
//signature.
    UINT8 PartitionSignature[16];

    UINT8 PartitionFormat;
// 40 1 Partition Format: (Unused values reserved) 15E79679 99999999
//0x01 – PC-AT compatible legacy MBR (see Section 5.2.1).
//Partition Start and Partition Size come from
//PartitionStartingLBAand PartitionSizeInLBAfor
//the partition.
//0x02 – GUID Partition Table (see Section 5.3.2).
    UINT8 SignatureType;
//41 1 Type of Disk Signature: (Unused values reserved)
//0x00 – No Disk Signature.
//0x01 – 32-bit signature fromaddress 0x1b8 of the type
//0x01 MBR.
//0x02 – GUID signature.
}MediaHDD;



size_t DumpHeader(UINT8 *pPath){
    UINT8 Type = *pPath;
    qDebug("Type: %x", Type);
    pPath++;
    UINT8 SubType = *pPath;
    qDebug("SubType: %x", SubType);
    pPath++;
    UINT16 Length = *(UINT16*)pPath;
    qDebug("Length: %x", Length);
    pPath+=2;

    int pathLen = Length - 4;
    wchar_t *buffer = new wchar_t[pathLen];
    switch (Type) {
    case 4:
        switch (SubType){
        case 4:
            memcpy_s(buffer, pathLen, pPath, pathLen);
            buffer[pathLen-1] = 0;
            qDebug()<<QString().fromWCharArray(buffer);
            break;
        case 1:
            MediaHDD md;
            md = *((MediaHDD *)pPath);
            qDebug()<<"PartitionNumber"<<md.PartitionNumber
                   <<"PartitionStart"<<md.PartitionStart
                  <<"PartitionSize"<<md.PartitionSize
                 <<"PartitionFormat"<<md.PartitionFormat
                <<"SignatureType"<<md.SignatureType;
            qDebug("PartitionSignature: %x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
                    md.PartitionSignature[0], md.PartitionSignature[1],
                    md.PartitionSignature[2], md.PartitionSignature[3],
                    md.PartitionSignature[4], md.PartitionSignature[5],
                    md.PartitionSignature[5], md.PartitionSignature[7],
                    md.PartitionSignature[8], md.PartitionSignature[9],
                    md.PartitionSignature[10], md.PartitionSignature[11],
                    md.PartitionSignature[12], md.PartitionSignature[13],
                    md.PartitionSignature[14], md.PartitionSignature[15]);
            break;
        }
        break;
    }
    return Length;
}

int UnpackBootXXXX(const QString &bootxxxx) {
    BYTE Val[4096];
    DWORD dwLen = 0;
    const TCHAR guid[] = TEXT("{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}");
//    const TCHAR system[] = TEXT("System");
//    const TCHAR sys_guid[] = TEXT("{E947FCf9-DD01-4965-B808-32A7B6815657}");
    qDebug()<<QString("\nUnpack %1").arg(bootxxxx);

    RasiePrivilegesXXX();

    dwLen = GetFirmwareEnvironmentVariable(
                bootxxxx.toStdWString().c_str(), guid, Val, 4096);

    if (dwLen == 0)
    {
        DWORD dwErr = GetLastError();
        qDebug("Failed, GetFirmwareEnvironmentVariable(), GetLastError return %d(0x%08x)\r\n",
            dwErr, dwErr);
        return 1;
    }

    qDebug()<<"dWLen"<<dwLen;
    //Attributes
    BYTE *pData = Val;
    UINT32 Attributes = *((UINT32*)pData);
    qDebug("Attributes: %x", Attributes);
    pData += 4;

    UINT16 FilePathListLength = *((UINT16*)pData);
    qDebug()<<"FilePathListLength: "<<FilePathListLength;
    pData += 2;

    char16_t *pDescription = (char16_t *)pData;

    wchar_t Description[1024];
    size_t i = 0;
    while(*pDescription != 0 && i < (dwLen/2)) {
        Description[i] = *pDescription;
        pDescription++;
        ++i;
    }
    size_t DescriptionLen = i;
    qDebug()<<"DescriptionLen"<<DescriptionLen;
    Description[i] = 0;
    ++pDescription;
    qDebug()<<QString().fromWCharArray(Description).toLatin1();
    pData = (BYTE*)pDescription;
    UINT8 FilePathList[1024];
    size_t bufLen = dwLen - 6 - (DescriptionLen + 1)*2;
    qDebug()<<"bufLen"<<bufLen;
    memcpy_s(FilePathList, FilePathListLength, pData, FilePathListLength);
    //qDebug()<<QString().fromLatin1(&FilePathList[0]).toLatin1();

    UINT8 *pPath = &FilePathList[0];
    UINT8 Type = *pPath;

    size_t readSize = 0;
    while (FilePathListLength > readSize){
        readSize += DumpHeader(pData+readSize);
        qDebug()<<readSize;
    }

    return 0;
    qDebug("Type: %x", Type);
    pPath++;
    UINT8 SubType = *pPath;
    qDebug("SubType: %x", SubType);
    pPath++;
    UINT16 Length = *(UINT16*)pPath;
    qDebug("Length: %x", Length);
    pPath+=2;


    UINT32 hid = 0;
    UINT64 uid = 0;
    MediaHDD md;
    switch (Type) {
    case 1:
        hid = *(UINT32*)pPath;
        qDebug("memtype: %x", hid);
        pPath += 4;
        uid = *(UINT64*)pPath;
        qDebug("Begin: %x", uid);
        pPath += 8;
        uid = *(UINT64*)pPath;
        qDebug("End: %x", uid);
        break;
    case 4:
        md = *((MediaHDD *)pPath);
        qDebug()<<md.PartitionNumber
               <<md.PartitionStart
              <<md.PartitionSize
             <<md.PartitionFormat
            <<md.SignatureType;
        break;
    }
    return 0;
}



