#include "Firmware.h"

#include "BootOption.h"
#include "BootOrder.h"
#include "GUID.h"

#include <xsys.h>

#include <iostream>

#include <Windows.h>
#include <QDebug>

using namespace std;

namespace Utils {

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX& partInfo);

void RasiePrivileges();

}

namespace UEFI{

std::wstring InsertBootOption(
       const std::wstring &target,
       const std::wstring &descrption,
       const std::wstring &imagepath
        ) {
    Utils::RasiePrivileges();
    BootOrder btorder;

    UINT16 bootIndex = btorder.NewIndex();

    wchar_t bootxxxx[32] = {0};
    swprintf_s(bootxxxx, 32, L"Boot%04x", bootIndex);

    BootOption btop(target, descrption, imagepath);
    DWORD bufferSize = DWORD(btop.Size());
    qDebug()<<"BootOption Size"<<bufferSize;
    UINT8* buffer = new UINT8[bufferSize];
    if (!btop.Pack(buffer, bufferSize)) {
        return L"";
    }
    qDebug()<<"Set New Boot Option"<<bootxxxx;
    if (SetFirmwareEnvironmentVariable(bootxxxx,
                                       UEFI_GUID,
                                       buffer,
                                       bufferSize) == TRUE) {
            qDebug()<<"Success!! SetFirmwareEnvironmentVariable"<<endl;
        } else {
            qDebug()<<"Failed!! SetFirmwareEnvironmentVariable"<<endl;
    }

    qDebug()<<"Insert New Boot Option"<<bootxxxx;
    btorder.Insert(bootIndex);
    delete buffer;
    return bootxxxx;
}

void RemoveBootOption(std::wstring bootxxxx){
    Utils::RasiePrivileges();

    if (SetFirmwareEnvironmentVariable(bootxxxx.c_str(),
                                   UEFI_GUID,
                                   NULL,
                                   NULL) == TRUE) {
        qDebug()<<"Success!! SetFirmwareEnvironmentVariable"<<endl;
    } else {
        qDebug()<<"Failed!! SetFirmwareEnvironmentVariable"<<GetLastError ()<<endl;
        return;
    }
    BootOrder btorder;
    UINT16 index = 0;
    for(size_t i = 4; i < 8; ++i) {
        index = index * 16 + (bootxxxx[i] - L'0');
    }
    btorder.Remove(index);
}

}
