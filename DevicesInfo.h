#pragma once

#ifdef USE_CLEW
    #include "clew.h"
#else
    #if defined(__APPLE__) || defined(__MACOSX)
        #include <OpenCL/cl.h>
    #else
        #include <CL/cl.h>
    #endif
#endif

#include "DeviceInfo.h"
#include "EasyCL_export.h"

namespace easycl {
    class EasyCL_EXPORT DevicesInfo {
    public:
        static int getNumDevices();
        static int getNumGpus();
        static int getNumDevices(int types);
        static DeviceInfo getDeviceInfo(int index);
        static DeviceInfo getGpuInfo(int gpuIndex);
        static DeviceInfo getDeviceInfo(int Index, int types);
        static void getDeviceIds(int index, int types, cl_platform_id *p_platformId, cl_device_id *p_deviceId);

        static void getIdForIndexedGpu(int gpu, cl_platform_id *p_platformId, cl_device_id *p_deviceId);
        static void getIdForIndexedDevice(int device, cl_platform_id *p_platformId, cl_device_id *p_deviceId);
        static void getIdForIndexedPlatformDevice(int platform, int device, int types, cl_platform_id *p_platformId, cl_device_id *p_deviceId);
    };
}
