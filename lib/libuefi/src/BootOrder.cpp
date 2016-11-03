#include "BootOrder.h"

#include "GUID.h"

#include <iostream>

#include <Windows.h>
#include <QDebug>
using namespace std;

namespace UEFI {

const wchar_t BootOrderStr[] = L"BootOrder";

const std::vector<UINT16>& BootOrder::GetOrders() {
    m_Orders = std::vector<UINT16>();

    const size_t bufSize = 4096;
    UINT8 buffer[bufSize];
    DWORD orderLen = GetFirmwareEnvironmentVariable(
        BootOrderStr, UEFI_GUID, buffer, bufSize);

    if (0 == orderLen)
    {
        qDebug()<<"Failed, GetFirmwareEnvironmentVariable(), GetLastError return "<<GetLastError();
        return m_Orders;
    }
    for(UINT32 i = 0; i < orderLen/2; i++) {
        qDebug()<<"Dump BootOrder: "<<hex<<((UINT16*)buffer)[i];
        m_Orders.push_back(((UINT16*)buffer)[i]);
    }
    return m_Orders;
}

bool BootOrder::Insert(UINT16 bootIndex){
    this->GetOrders();
    foreach (UINT16 item, m_Orders){
        if (bootIndex == item){
            return true;
        }
    }

    std::vector<UINT16> newOrders ;
    newOrders.push_back(bootIndex);
    for(size_t i=0; i < m_Orders.size(); i++) {
        newOrders.push_back(m_Orders[i]);
    }
    m_Orders = newOrders;
    return this->SetOrders();
}

bool BootOrder::Remove(UINT16 bootIndex){
    this->GetOrders();
    m_Orders.erase(find(m_Orders.begin(), m_Orders.end(), bootIndex));
    return this->SetOrders();
}

UINT16 BootOrder::NewIndex(){
    this->GetOrders();
    UINT16 newIndex = 32;
    while(true){
        bool dumpIndex = false;
        for(size_t i=0; i < m_Orders.size(); i++) {
            if (newIndex == m_Orders[i]) {
                dumpIndex = true;
            }
        }
        if (!dumpIndex)  {
            qDebug()<<"Find NewIndex"<<newIndex;
            return newIndex;
        }
        ++newIndex;
    }
}

bool BootOrder::SetOrders() {
    const size_t bufSize = 4096;
    UINT8 buffer[bufSize];
    size_t index = 0;
    foreach (UINT16 item, m_Orders){
        ((IndexValue*)buffer)[index] = item;
        index++;
    }

    if (!SetFirmwareEnvironmentVariable(BootOrderStr,
                                       UEFI_GUID,
                                       buffer,
                                       DWORD(m_Orders.size()*sizeof(IndexValue)/sizeof(UINT8))) == TRUE) {
            qDebug()<<"Failed!! SetFirmwareEnvironmentVariable"<<GetLastError ();
            return false;
    }
    qDebug()<<"Success!! SetFirmwareEnvironmentVariable";
    return true;
}

}
