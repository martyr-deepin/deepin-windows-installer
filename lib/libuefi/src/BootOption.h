#pragma once

#include <vector>
#include <string>

#include <Windows.h>

#pragma pack(1)

typedef struct {
    UINT32 Attributes;
    UINT16 FilePathListLength;
    //WCHAR  Description[1];
} EFI_LOAD_OPTION;

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

  UINT16 Length;  ///< Specific Device Path data. Type and Sub-Type define
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


typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  //WCHAR                           PathName[1];
} FILEPATH_DEVICE_PATH;


namespace UEFI{

class BootOption {
public:
    BootOption(const std::wstring target,
               const std::wstring descrption,
               const std::wstring imagepath);

    size_t Size();
    bool Pack(unsigned char *buffer, size_t bufferLen);

private:
    EFI_LOAD_OPTION         Header;
    HARDDRIVE_DEVICE_PATH   DevicePath;
    FILEPATH_DEVICE_PATH    ImagePath;
    EFI_DEVICE_PATH_PROTOCOL Butt;

    UINT8        *pFilePathList;
    std::wstring m_Target;
    std::wstring m_Description;
    std::wstring m_ImagePath;
};

}


