// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <cassert>
#include <cstdlib>
using namespace std;

#ifdef USE_CLEW
#include "clew.h"
#else
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#endif

#include "deviceinfo_helper.h"
#include "platforminfo_helper.h"

using namespace easycl;

int main(int argc, char *argv[]) {
    #ifdef USE_CLEW
        bool clpresent = 0 == clewInit();
        if(!clpresent) {
            cout << "opencl library not found." << endl;
            return -1;
        }
    #endif

    cl_int error = 0;   // Used to handle error codes
    cl_platform_id platform_ids[10];
//    cl_context context;
//    cl_command_queue queue;
    cl_device_id device;

    // Platform
    cl_uint num_platforms;
    error = clGetPlatformIDs(10, platform_ids, &num_platforms);
    cout << "num platforms: " << num_platforms << endl;
    if(num_platforms == 0) {
        cout << "No platforms found => exiting" << endl;
        return 0;
    }
    assert (error == CL_SUCCESS);
    cout << endl;

    for(int i = 0; i < (int)num_platforms; i++) {
        cout << "platform index: " << i << ":" << endl;
        cl_platform_id platform_id = platform_ids[i];
        cout << "platform id: " << platform_id << endl;
        printPlatformInfoString("platform vendor", platform_id, CL_PLATFORM_VENDOR);
        printPlatformInfoString("platform name", platform_id, CL_PLATFORM_NAME);
        cl_uint num_devices;
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 0, 0, &num_devices);
        if (error != CL_SUCCESS) {
           cout << "Error getting device ids: " << error << endl;
           exit(error);
        }
        cout << "platform num devices: " << num_devices << endl;
        cout << endl;
        cl_device_id *device_ids = new cl_device_id[num_devices];
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, num_devices, device_ids, &num_devices);
        if (error != CL_SUCCESS) {
           cout << "Error getting device ids: " << error << endl;
           exit(error);
        }
        for(int i = 0; i < (int)num_devices; i++) {
            device = device_ids[i];
            cout << "   device index: " << i << endl;
            cout << "   device id: " << device << endl;
            printDeviceInfo("   device type", device, CL_DEVICE_TYPE);
            printDeviceInfoMB("   global memory size", device, CL_DEVICE_GLOBAL_MEM_SIZE);
            printDeviceInfoKB("   local memory size", device, CL_DEVICE_LOCAL_MEM_SIZE);
            printDeviceInfoKB("   global cache size", device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
            printDeviceInfo("   global cacheline size", device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
            printDeviceInfoMB("   max memory alloc size", device, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
            printDeviceInfo("   max compute units", device, CL_DEVICE_MAX_COMPUTE_UNITS);
            printDeviceInfo("   max workgroup size", device, CL_DEVICE_MAX_WORK_GROUP_SIZE);
            printDeviceInfo("   max workitem dimensions", device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
            printDeviceInfoArray("   max workitem sizes", device, CL_DEVICE_MAX_WORK_ITEM_SIZES, 3);
            printDeviceInfoString("   device name", device, CL_DEVICE_NAME);
            printDeviceInfoString("   opencl c version", device, CL_DEVICE_OPENCL_C_VERSION);
            printDeviceInfoString("   opencl device version", device, CL_DEVICE_VERSION);
            printDeviceInfo("   frequency MHz", device, CL_DEVICE_MAX_CLOCK_FREQUENCY);
            cout << endl;
        }
        delete[] device_ids;
    }
    return 0;
}
