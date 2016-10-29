// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>
#include <string>

#ifdef USE_CLEW
    #include "clew.h"
#else
    #if defined(__APPLE__) || defined(__MACOSX)
        #include <OpenCL/cl.h>
    #else
        #include <CL/cl.h>
    #endif
#endif

#include "mystdint.h"

namespace easycl {
void printDeviceInfoKB(std::string valuename, cl_device_id deviceId, cl_device_info name);
void printDeviceInfoMB(std::string valuename, cl_device_id deviceId, cl_device_info name);
void printDeviceInfo(std::string valuename, cl_device_id deviceId, cl_device_info name);
void printDeviceInfoArray(std::string valuename, cl_device_id deviceId, cl_device_info name, int length);
void printDeviceInfoString(std::string valuename, cl_device_id deviceId, cl_device_info name);

std::string getDeviceInfoString(cl_device_id deviceId, cl_device_info name);
int getDeviceInfoInt(cl_device_id deviceId, cl_device_info name);
int64_t getDeviceInfoInt64(cl_device_id deviceId, cl_device_info name);
bool getDeviceInfoBool(cl_device_id deviceId, cl_device_info name);
}
