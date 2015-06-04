#pragma once

#include <string>
#include "DeviceInfo.h"
#include "clew.h"

namespace easycl {
    class DeviceInfo {
    public:
//        long platformId;
        std::string platformVendor;
        std::string platformName;
//        long deviceId;
        int deviceType;
        long globalMemSize;
        int localMemSize;
        int globalMemCachelineSize;
        long maxMemAllocSize;
        int maxComputeUnits;
        int maxWorkGroupSize;
        int maxWorkItemDimensions;
    //    int maxWorkItemSizes;
        std::string deviceName;
        std::string openClCVersion;
        std::string deviceVersion;
        int maxClockFrequency;

        void populate( cl_platform_id platformId, cl_device_id deviceId );
        std::string toString();
    };
}

