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

#include "EasyCL_export.h"

namespace easycl {
EasyCL_EXPORT void printDeviceInfoKB(std::string valuename, cl_device_id deviceId, cl_device_info name);
EasyCL_EXPORT void printDeviceInfoMB(std::string valuename, cl_device_id deviceId, cl_device_info name);
EasyCL_EXPORT void printDeviceInfo(std::string valuename, cl_device_id deviceId, cl_device_info name);
EasyCL_EXPORT void printDeviceInfoArray(std::string valuename, cl_device_id deviceId, cl_device_info name, int length);
EasyCL_EXPORT void printDeviceInfoString(std::string valuename, cl_device_id deviceId, cl_device_info name);

EasyCL_EXPORT std::string getDeviceInfoString(cl_device_id deviceId, cl_device_info name);
EasyCL_EXPORT int getDeviceInfoInt(cl_device_id deviceId, cl_device_info name);
EasyCL_EXPORT int64_t getDeviceInfoInt64(cl_device_id deviceId, cl_device_info name);
EasyCL_EXPORT bool getDeviceInfoBool(cl_device_id deviceId, cl_device_info name);
}
