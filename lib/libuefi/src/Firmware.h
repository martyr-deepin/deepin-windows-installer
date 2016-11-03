#pragma once

#include <vector>
#include <string>

namespace UEFI{

std::wstring InsertBootOption(
       const std::wstring &target,
       const std::wstring &descrption,
       const std::wstring &imagepath);

void RemoveBootOption(std::wstring bootxxxx);

}

#include <Windows.h>

namespace Utils {

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX& partInfo);

}
