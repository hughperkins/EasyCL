#pragma once

#include "clew.h"
#include "DeviceInfo.h"

namespace easycl {
    class DevicesInfo {
    public:
        static int getNumDevices();
        static int getNumGpus();
        static int getNumDevices( int types );
        static DeviceInfo getDeviceInfo( int index );
        static DeviceInfo getGpuInfo( int gpuIndex );
        static DeviceInfo getDeviceInfo( int Index, int types );
        static void getDeviceIds( int index, int types, cl_platform_id *p_platformId, cl_device_id *p_deviceId );
    };
}

