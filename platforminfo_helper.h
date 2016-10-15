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

namespace easycl {
void printPlatformInfoString(std::string valuename, cl_platform_id platformId, cl_platform_info name);
void printPlatformInfo(std::string valuename, cl_platform_id platformId, cl_platform_info name);
std::string getPlatformInfoString(cl_platform_id platformId, cl_platform_info name);
}
