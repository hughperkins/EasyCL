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
#include "DevicesInfo.h"
#include "deviceinfo_helper.h"
#include "platforminfo_helper.h"
#include <stdexcept>

using namespace std;

namespace easycl {
    std::string errorMessage(cl_int error) {
        return EasyCL::toString(error);
    }
    DeviceInfo DevicesInfo::getGpuInfo(int index) {
        return getDeviceInfo(index, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR);
    }
    DeviceInfo DevicesInfo::getDeviceInfo(int index) {
        return getDeviceInfo(index, CL_DEVICE_TYPE_ALL);
    }
    DeviceInfo DevicesInfo::getDeviceInfo(int index, int types) {
        cl_platform_id platformId;
        cl_device_id deviceId;
        getDeviceIds(index, types, &platformId, &deviceId);
        DeviceInfo info;
        info.populate(platformId, deviceId);
        return info;
    }
    void DevicesInfo::getDeviceIds(int index, int types, cl_platform_id *p_platformId, cl_device_id *p_deviceId) {
        #ifdef USE_CLEW
            bool clpresent = 0 == clewInit();
            if(!clpresent) {
                throw std::runtime_error("OpenCL library not found");
            }
        #endif
        cl_int error;
        int currentGpuIndex = 0;
        cl_platform_id platform_ids[10];
        cl_uint num_platforms;
        error = clGetPlatformIDs(10, platform_ids, &num_platforms);
        if (error != CL_SUCCESS) {
           throw std::runtime_error("Error getting platforms ids: " + errorMessage(error));
        }
        if(num_platforms == 0) {
           throw std::runtime_error("Error: no platforms available");
        }
//        int numDevices = 0;
        for(int platform =  0; platform < (int)num_platforms; platform++) {
            cl_platform_id platform_id = platform_ids[platform];
            cout << "checking platform id " << platform_id << endl;
//            cl_device_id device_ids[100];
            cl_uint num_devices;
            error = clGetDeviceIDs(platform_id, types, 0, 0, &num_devices);
            if (error != CL_SUCCESS) {
//                continue;
               throw std::runtime_error("Error getting num devices for platform " + EasyCL::toString(platform) + ": " +
                   errorMessage(error));
            }
            cout << "num devices this platform " << num_devices << endl;
            cl_device_id *device_ids = new cl_device_id[num_devices];
            error = clGetDeviceIDs(platform_id, types, num_devices, device_ids, &num_devices);
            if (error != CL_SUCCESS) {
//                continue;
               throw std::runtime_error("Error getting device ids for platform " + EasyCL::toString(platform) + ": " + errorMessage(error));
            }
            if(( index - currentGpuIndex) < (int)num_devices) {
                *p_platformId = platform_id;
                *p_deviceId  = device_ids[(index - currentGpuIndex)];
                delete[] device_ids;
                return;
            } else {
                delete[] device_ids;
                currentGpuIndex += num_devices;
            }
        }
        if(index == 0) {
            throw std::runtime_error("No devices found");
        } else {
            throw std::runtime_error("Not enough devices found to satisfy index: " + EasyCL::toString(index) );
        }
    }
    int DevicesInfo::getNumGpus() {
        return getNumDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR);
    }
    int DevicesInfo::getNumDevices() {
        return getNumDevices(CL_DEVICE_TYPE_ALL);
    }
    int DevicesInfo::getNumDevices(int types) {
        #ifdef USE_CLEW
            bool clpresent = 0 == clewInit();
            if(!clpresent) {
                throw std::runtime_error("OpenCL library not found");
            }
        #endif
        cl_int error;
//        int currentGpuIndex = 0;
        cl_platform_id platform_ids[10];
        cl_uint num_platforms;
        error = clGetPlatformIDs(10, platform_ids, &num_platforms);
        if (error != CL_SUCCESS) {
            return 0;
//           throw std::runtime_error("Error getting platforms ids: " + errorMessage(error));
        }
        cout << "num platforms " << num_platforms << endl;
        if(num_platforms == 0) {
            return 0;
//           throw std::runtime_error("Error: no platforms available");
        }
        int numDevices = 0;
        for(int platform =  0; platform < (int)num_platforms; platform++) {
            cl_platform_id platform_id = platform_ids[platform];
            cout << "checking platform id " << platform_id << endl;
            cl_uint num_devices;
            error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 0, 0, &num_devices);
            if (error != CL_SUCCESS) {
                continue;
//                throw std::runtime_error("Error getting num device ids for platform " + EasyCL::toString(platform) + ": " + errorMessage(error));
            }
            cout << "num devices " << num_devices << endl;
//            cl_device_id *device_ids = new cl_device_id[num_devices];
//            error = clGetDeviceIDs(platform_id, types, num_devices, device_ids, &num_devices);
//            cout << "num devices " << num_devices << endl;
//            if (error != CL_SUCCESS) {
////                continue;
//                throw std::runtime_error("Error getting device ids for platform " + EasyCL::toString(platform) + ": " + errorMessage(error));
//            }
            numDevices += num_devices;
//            delete[] device_ids
        }
        return numDevices;
    }
    void DevicesInfo::getIdForIndexedGpu(int gpu, cl_platform_id *p_platformId, cl_device_id *p_deviceId) {
        getDeviceIds(gpu, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR, p_platformId, p_deviceId);
    }
    void DevicesInfo::getIdForIndexedDevice(int device, cl_platform_id *p_platformId, cl_device_id *p_deviceId) {
        getDeviceIds(device, CL_DEVICE_TYPE_ALL, p_platformId, p_deviceId);
    }
    void DevicesInfo::getIdForIndexedPlatformDevice(int platform, int device, int types, cl_platform_id *p_platformId, cl_device_id *p_deviceId) {
        #ifdef USE_CLEW
            bool clpresent = 0 == clewInit();
            if(!clpresent) {
                throw std::runtime_error("OpenCL library not found");
            }
        #endif
        cl_int error;
        cl_platform_id platform_ids[10];
        cl_uint num_platforms;
        error = clGetPlatformIDs(10, platform_ids, &num_platforms);
        if (error != CL_SUCCESS) {
            throw std::runtime_error("Error getting platforms ids: " + errorMessage(error));
        }
        if(num_platforms == 0) {
            throw std::runtime_error("Error: no platforms available");
        }
        if(platform < 0) {
            throw std::runtime_error("Error: platform index must be non-negative");
        }
        if(platform >= (int)num_platforms) {
            throw runtime_error("Error: not enough platforms available to satisfy index");
        }
        cl_platform_id platform_id = platform_ids[platform];        
        cl_device_id device_ids[100];
        cl_uint num_devices;
        error = clGetDeviceIDs(platform_id, types, 100, device_ids, &num_devices);
        if (error != CL_SUCCESS) {
            throw std::runtime_error("Error getting device ids for platform " + EasyCL::toString(platform) + ": " + errorMessage(error));
        }
        if(device < 0) {
            throw runtime_error("Error: device must be non-negative");
        }
        if(device >= (int)num_devices) {
            throw runtime_error("Error: device index greater than number available devices");
        }
        *p_platformId = platform_id;
        *p_deviceId  = device_ids[device];
    }
}

