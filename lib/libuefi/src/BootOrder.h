#pragma once

#include <vector>

#include "Type.h"

namespace UEFI {

class BootOrder {
public:
    bool Insert(UINT16 bootIndex);
    bool Remove(UINT16 bootIndex);
    UINT16 NewIndex();

private:
    const std::vector<UINT16>& GetOrders();
    bool SetOrders();

    typedef UINT16 IndexValue;
    typedef std::vector<UINT16>::iterator IndexIter;

    std::vector<UINT16> m_Orders;
};

}
