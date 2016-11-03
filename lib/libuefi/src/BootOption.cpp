#include "BootOption.h"

#include <xsys.h>
#include <string>

#include <QDebug>

namespace Utils {

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX& partInfo);

}

namespace UEFI {

BootOption::BootOption(const std::wstring target,
               const std::wstring descrption,
               const std::wstring imagepath) {
    m_Target = target;
    m_Description = descrption;
    m_ImagePath = imagepath;
    Header.Attributes = 1;
    Header.FilePathListLength =
            sizeof(DevicePath)
            + sizeof(ImagePath)
            + UINT16(m_ImagePath.length()+1) * 2 + 4;

    DevicePath.Header.Type = 4;
    DevicePath.Header.SubType = 1;
    DevicePath.Header.Length = 42;
    PARTITION_INFORMATION_EX part;
    Utils::GetPartitionInfo(target, part);
    DevicePath.PartitionNumber = part.PartitionNumber;
    DevicePath.PartitionStart = part.StartingOffset.QuadPart / 512;
    DevicePath.PartitionSize = part.PartitionLength.QuadPart / 512;
    DevicePath.MBRType = 2;
    DevicePath.SignatureType = 2;
    memcpy_s(DevicePath.Signature, 16, &(part.Gpt.PartitionId), 16);

    ImagePath.Header.Type = 4;
    ImagePath.Header.SubType = 4;
    ImagePath.Header.Length = 4 + UINT16(m_ImagePath.length()+1)*2;

    Butt.Type = 0x7F;
    Butt.SubType = 0xFF;
    Butt.Length = 4;
}

size_t BootOption::Size(){
    return sizeof(Header)
            + m_Description.length()*2 + 2
            + sizeof(DevicePath)
            + sizeof(ImagePath)
            + (m_ImagePath.length()+1)*2
            + 4;
}

bool BootOption::Pack(unsigned char *buffer, size_t /*bufferLen*/) {
    //Att
    ((UINT32 *)buffer)[0] = Header.Attributes;
    qDebug()<<"Header.Attributes"<<Header.Attributes;
    buffer += 4;
    ((UINT16 *)buffer)[0] = Header.FilePathListLength;
    qDebug()<<"Header.FilePathListLength"<<Header.FilePathListLength;
    buffer += 2;

    //Description
    size_t descriptionSize = m_Description.length()*2;
    qDebug()<<m_Description.c_str()
            <<"descriptionSize"
            <<descriptionSize;
    for (size_t i =0; i < descriptionSize/2; ++i) {
        *((wchar_t*)buffer+i) = m_Description[i];
    }

    buffer += descriptionSize;
    *(wchar_t*)buffer = 0;
    buffer += 2;

    //DevicePath
    memcpy_s(buffer, DevicePath.Header.Length,
             &DevicePath, DevicePath.Header.Length);
    buffer += DevicePath.Header.Length;

    memcpy_s(buffer, 4, &ImagePath, 4);
    buffer += 4;
    size_t imagepathSize = m_ImagePath.length()*2;
    qDebug()<<"imagepathSize"<<imagepathSize;
    for (size_t i =0; i < imagepathSize/2; ++i) {
     *((wchar_t*)buffer+i) = m_ImagePath[i];
    }
    buffer += imagepathSize;
    *(wchar_t*)buffer = 0;
    buffer += 2;

    qDebug()<<(m_ImagePath.c_str())
            <<"imagepathSize"
            <<imagepathSize;
    memcpy_s(buffer, 4, &Butt, 4);
    return true;
}

}
