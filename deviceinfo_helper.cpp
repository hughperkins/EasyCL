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

#include "EasyCL.h"

#include "deviceinfo_helper.h"

using namespace std;

namespace easycl {

void printDeviceInfoKB(string valuename, cl_device_id deviceId, cl_device_info name) {
    cl_ulong somelong = 0;
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << (somelong/1024) << "KB" << endl;    
}

void printDeviceInfoMB(string valuename, cl_device_id deviceId, cl_device_info name) {
    cl_ulong somelong = 0;
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << (somelong/1024/1024) << "MB" << endl;    
}

void printDeviceInfo(string valuename, cl_device_id deviceId, cl_device_info name) {
    cl_ulong somelong = 0;
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << somelong << endl;    
}

void printDeviceInfoArray(string valuename, cl_device_id deviceId, cl_device_info name, int length) {
    cl_ulong *array = new cl_ulong[length];
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong)*length, array, 0);
    cout << valuename << ":";
    for(int i = 0; i < length; i++) {
        cout << " " << array[i];   
    }
    cout << endl;
    delete[] array;
}

void printDeviceInfoString(string valuename, cl_device_id deviceId, cl_device_info name) {
    char buffer[256];
    buffer[0] = 0;
    clGetDeviceInfo(deviceId, name, 256, buffer, 0);
    cout << valuename << ": " << buffer << endl;    
}

string getDeviceInfoString(cl_device_id deviceId, cl_device_info name) {
    char buffer[256];
    buffer[0] = 0;
    cl_int error = clGetDeviceInfo(deviceId, name, 256, buffer, 0);
    if(error != CL_SUCCESS) {
        if(error == CL_INVALID_DEVICE) {
            throw runtime_error("Failed to obtain info for device id " + EasyCL::toString(deviceId) + ": invalid device");
        } else if(error == CL_INVALID_VALUE) {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": invalid value");
        } else {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": unknown error code: " + EasyCL::toString(error) );
        }
    }
    return string(buffer);
}
int getDeviceInfoInt(cl_device_id deviceId, cl_device_info name) {
    cl_ulong somelong = 0;
    cl_int error = clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    if(error != CL_SUCCESS) {
        if(error == CL_INVALID_DEVICE) {
            throw runtime_error("Failed to obtain info for device id " + EasyCL::toString(deviceId) + ": invalid device");
        } else if(error == CL_INVALID_VALUE) {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": invalid value");
        } else {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": unknown error code: " + EasyCL::toString(error) );
        }
    }
    return (int)somelong;
}
int64_t getDeviceInfoInt64(cl_device_id deviceId, cl_device_info name) {
    int64_t somelong = 0;
    cl_int error = clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    if(error != CL_SUCCESS) {
        if(error == CL_INVALID_DEVICE) {
            throw runtime_error("Failed to obtain info for device id " + EasyCL::toString(deviceId) + ": invalid device");
        } else if(error == CL_INVALID_VALUE) {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": invalid value");
        } else {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": unknown error code: " + EasyCL::toString(error) );
        }
    }
    return somelong;
}
bool getDeviceInfoBool(cl_device_id deviceId, cl_device_info name) {
    cl_bool val = false;
    cl_int error = clGetDeviceInfo(deviceId, name, sizeof(val), &val, 0);
    if(error != CL_SUCCESS) {
        if(error == CL_INVALID_DEVICE) {
            throw runtime_error("Failed to obtain info for device id " + EasyCL::toString(deviceId) + ": invalid device");
        } else if(error == CL_INVALID_VALUE) {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": invalid value");
        } else {
            throw runtime_error("Failed to obtain device info " + EasyCL::toString(name) + " for device id " + EasyCL::toString(deviceId) + ": unknown error code: " + EasyCL::toString(error) );
        }
    }
    return (bool)val;
}

}
