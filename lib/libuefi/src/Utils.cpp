#include <Windows.h>

#include <iostream>
#include <algorithm>
#include <functional>

#include <QDebug>

static bool GetPartitionByHandle(HANDLE handle, PARTITION_INFORMATION_EX& partInfo){
   DWORD count;
   if (!DeviceIoControl(
     (HANDLE) handle,                   // handle to a partition
     IOCTL_DISK_GET_PARTITION_INFO_EX,  // dwIoControlCode
     NULL,                              // lpInBuffer
     0,                                 // nInBufferSize
     (LPVOID) &partInfo,                // output buffer
     (DWORD) sizeof(partInfo),          // size of output buffer
     (LPDWORD) &count,                  // number of bytes returned
     (LPOVERLAPPED) NULL                // OVERLAPPED structure
   )){
       return false;
   }
   return true;
}


namespace Utils {

using namespace std;

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX& partInfo){
    wstring::iterator new_end = remove_if(targetDev.begin(),
                                        targetDev.end(),
                                        bind2nd(equal_to<wchar_t>(),
                                        '\\'));
    targetDev = L"\\\\.\\" + targetDev;
    qDebug()<<QString().fromStdWString(targetDev);
    HANDLE handle = CreateFile(targetDev.c_str(), GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ|FILE_SHARE_WRITE,
              NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        qDebug()<<"Open Dev Failed: "<<GetLastError()<<handle<<endl;
        return false;
    }
    return GetPartitionByHandle(handle, partInfo);
}

void RasiePrivileges(void)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(),
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                        &hToken)) {
        cout<<"Failed OpenProcessToken"<<endl;
        return;
    }

    LookupPrivilegeValue(NULL,
                         SE_SYSTEM_ENVIRONMENT_NAME,
                         &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    DWORD len;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, &len);

    if (GetLastError() != ERROR_SUCCESS) {
        cout<<"Failed RasiePrivileges()"<<endl;
        return;
    }
}


}
