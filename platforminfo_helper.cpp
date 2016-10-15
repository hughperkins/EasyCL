// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#ifdef USE_CLEW
#include "clew.h"
#endif

#include <algorithm>
#include <string>
#include <iostream>
#include <stdexcept>

#include "EasyCL.h"

#include "platforminfo_helper.h"

using namespace std;

namespace easycl {

void printPlatformInfoString(string valuename, cl_platform_id platformId, cl_platform_info name) {
    char buffer[256];
    buffer[0] = 0;
    clGetPlatformInfo(platformId, name, 256, buffer, 0);
    cout << valuename << ": " << buffer << endl;    
}

string getPlatformInfoString(cl_platform_id platformId, cl_platform_info name) {
    char buffer[257];
    buffer[0] = 0;
    size_t namesize;
    cl_int error = clGetPlatformInfo(platformId, name, 256, buffer, &namesize);
    if(error != CL_SUCCESS) {
        if(error == CL_INVALID_PLATFORM) {
            throw runtime_error("Failed to obtain platform info for platform id " + EasyCL::toString(platformId) + ": invalid platform");
        } else if(error == CL_INVALID_VALUE) {
            throw runtime_error("Failed to obtain platform info " + EasyCL::toString(name) + " for platform id " + EasyCL::toString(platformId) + ": invalid value");
        } else {
            throw runtime_error("Failed to obtain platform info " + EasyCL::toString(name) + " for platform id " + EasyCL::toString(platformId) + ": unknown error code: " + EasyCL::toString(error) );
        }
    }
    return string(buffer);
}

void printPlatformInfo(string valuename, cl_platform_id platformId, cl_platform_info name) {
    cl_ulong somelong = 0;
    clGetPlatformInfo(platformId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << somelong << endl;    
}

}
