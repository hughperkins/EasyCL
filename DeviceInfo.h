#pragma once

#include <string>
#include "DeviceInfo.h"
#include "clew.h"

#include "EasyCL_export.h"

typedef long long int64;

namespace easycl {
    class EasyCL_EXPORT DeviceInfo {
#ifdef _WIN32
#pragma warning(disable: 4251)
#endif
    public:
//        long platformId;
        std::string platformVendor;
        std::string platformName;
//        long deviceId;
        int deviceType;
        int64 globalMemSize;
        int localMemSize;
        int globalMemCachelineSize;
        int64 maxMemAllocSize;
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

