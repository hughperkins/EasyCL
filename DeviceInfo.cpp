#ifdef USE_CLEW
#include "clew.h"
#else
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#endif

#include "EasyCL.h"
#include "DeviceInfo.h"
#include "platforminfo_helper.h"
#include "deviceinfo_helper.h"

using namespace std;

namespace easycl {
    void DeviceInfo::populate(cl_platform_id platformId, cl_device_id deviceId) {
//        this->platformId = platformId;
        platformVendor = getPlatformInfoString(platformId, CL_PLATFORM_VENDOR);
        platformName = getPlatformInfoString(platformId, CL_PLATFORM_NAME);
//        this->deviceId = deviceId;
        deviceType = getDeviceInfoInt(deviceId, CL_DEVICE_TYPE);
        globalMemSize = getDeviceInfoInt64(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE);
        localMemSize = getDeviceInfoInt(deviceId, CL_DEVICE_LOCAL_MEM_SIZE);
        globalMemCachelineSize = getDeviceInfoInt(deviceId, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
        maxMemAllocSize = getDeviceInfoInt64(deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
        maxComputeUnits = getDeviceInfoInt(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS);
        maxWorkGroupSize = getDeviceInfoInt(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE);
        maxWorkItemDimensions = getDeviceInfoInt(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
    //    maxWorkItemSizes = getDeviceInfoInt(deviceId, CL_MAX_WORK_ITEM_SIZES);
        deviceName = getDeviceInfoString(deviceId, CL_DEVICE_NAME);
        openClCVersion = getDeviceInfoString(deviceId, CL_DEVICE_OPENCL_C_VERSION);
        deviceVersion = getDeviceInfoString(deviceId, CL_DEVICE_VERSION);
        maxClockFrequency = getDeviceInfoInt(deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY);
    }

    string DeviceInfo::toString() {
//        return "platformId: " + toString(platformId) + "\n"
            return "platformVendor: " + platformVendor + "\n"
            + "platformName: " + platformName + "\n"
//            + "deviceId: " + toString(deviceId) + "\n"
            + "deviceType: " + EasyCL::toString(deviceType) + "\n"
            + "globalMemSize: " + EasyCL::toString(globalMemSize) + "\n"
            + "localMemSize: " + EasyCL::toString(localMemSize) + "\n"
            + "globalMemCachelineSize: " + EasyCL::toString(globalMemCachelineSize) + "\n"
            + "maxMemAllocSize: " + EasyCL::toString(maxMemAllocSize) + "\n"
            + "maxComputeUnits: " + EasyCL::toString(maxComputeUnits) + "\n"
            + "maxWorkGroupSize: " + EasyCL::toString(maxWorkGroupSize) + "\n"
            + "maxWorkItemDimensions: " + EasyCL::toString(maxWorkItemDimensions) + "\n"
    //        + "maxWorkItemSizes: " + toString(maxWorkItemSizes) + "\n"
            + "deviceName: " + deviceName + "\n"
            + "openClCVersion: " + openClCVersion + "\n"
            + "deviceVersion: " + deviceVersion + "\n"
            + "maxClockFrequency: " + EasyCL::toString(maxClockFrequency) + "\n";
    }
}

