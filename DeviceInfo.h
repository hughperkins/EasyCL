#pragma once

#include <string>
#include "DeviceInfo.h"
#ifdef USE_CLEW
    #include "clew.h"
#else
    #if defined(__APPLE__) || defined(__MACOSX)
        #include <OpenCL/cl.h>
    #else
        #include <CL/cl.h>
    #endif
#endif

#include "EasyCL_export.h"

#include "mystdint.h"

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
        int64_t globalMemSize;
        int localMemSize;
        int globalMemCachelineSize;
        int64_t maxMemAllocSize;
        int maxComputeUnits;
        int maxWorkGroupSize;
        int maxWorkItemDimensions;
    //    int maxWorkItemSizes;
        std::string deviceName;
        std::string openClCVersion;
        std::string deviceVersion;
        int maxClockFrequency;

        void populate(cl_platform_id platformId, cl_device_id deviceId);
        std::string toString();
    };
}
