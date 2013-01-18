// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
using namespace std;

#include <CL/cl.h>

// This code is partly based on http://opencl.codeplex.com/wikipage?title=OpenCL%20Tutorials%20-%201 , with helpers added for input and output arguments
class OpenCLHelper {
public:
     cl_int error;  

    cl_platform_id platform_id;
    cl_context context;
    cl_command_queue queue;
    cl_device_id device;

    int gpuIndex;

    class CLArray1d *array1d(int N );

    ~OpenCLHelper() {
        clReleaseCommandQueue(queue);
        clReleaseContext(context);        
    }

    static int roundUp( int quantization, int minimum ) {
        int size = ( minimum / quantization) * quantization;
        if( size < minimum ) {
            size += quantization;
        }
        return size;
    }

    static int getPower2Upperbound( int value ) {
        int upperbound = 1;
        while( upperbound < value ) {
            upperbound <<= 1;
        }
        return upperbound;
    }

    OpenCLHelper(int gpuindex ) {
        this->gpuIndex = gpuindex;
        error = 0;

        // Platform
        cl_uint num_platforms;
        error = clGetPlatformIDs(1, &platform_id, &num_platforms);
        cout << "num platforms: " << num_platforms << endl;
        assert (num_platforms == 1);
        assert (error == CL_SUCCESS);

        cl_uint num_devices;
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
        if (error != CL_SUCCESS) {
           cout << "Error getting device ids: " << errorMessage(error) << endl;
           exit(error);
        }
        cout << "num devices: " << num_devices << endl;
        cl_device_id *device_ids = new cl_device_id[num_devices];
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, num_devices, device_ids, &num_devices);
        if (error != CL_SUCCESS) {
           cout << "Error getting device ids: " << errorMessage(error) << endl;
           exit(error);
        }
        assert( gpuindex < num_devices );
        device = device_ids[gpuindex];
        delete[] device_ids;

        // Context
        context = clCreateContext(0, 1, &device, NULL, NULL, &error);
        if (error != CL_SUCCESS) {
           cout << "Error creating context: " << errorMessage(error) << endl;
           exit(error);
        }
        // Command-queue
        queue = clCreateCommandQueue(context, device, 0, &error);
        if (error != CL_SUCCESS) {
           cout << "Error creating command queue: " << errorMessage(error) << endl;
           exit(error);
        }

    }

    void finish() {
        error = clFinish( queue );
        checkError(error);
    }

    class CLKernel *buildKernel( string kernelfilepath, string kernelname );

    int getComputeUnits() {
        return (int)getDeviceInfoInt(CL_DEVICE_MAX_COMPUTE_UNITS);
    }

    int getLocalMemorySize() {
        return (int)getDeviceInfoInt(CL_DEVICE_LOCAL_MEM_SIZE);
    }

    int getMaxWorkgroupSize() {
        return (int)getDeviceInfoInt(CL_DEVICE_MAX_WORK_GROUP_SIZE);
    }



static string errorMessage(cl_int error ) {
    return toString(error);
}

static void checkError( cl_int error ) {
    if( error != CL_SUCCESS ) {
        cout << "error: " << error << endl;
        assert (false);
    }
}

private:


static string getFileContents( string filename ) {
    char * buffer = 0;
    long length;
    FILE * f = fopen (filename.c_str(), "rb");

    string returnstring = "";
    if (f)
    {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = new char[length];
      if (buffer) {
        int bytesread = fread (buffer, 1, length, f);
        if( bytesread != length ) {
            cout << "Failed to read cl source file" << endl;
            exit(-1);
        }
      } else {
        cout << "Failed to allocate memory for cl source" << endl;
        exit(-1);
       }
      fclose (f);
      returnstring = buffer;
      delete[] buffer;
    }
    return returnstring;
}

template<typename T>
static std::string toString(T val ) {
   std::ostringstream myostringstream;
   myostringstream << val;
   return myostringstream.str();
}

long getDeviceInfoInt( cl_device_info name ) {
    cl_ulong value = 0;
    clGetDeviceInfo(device, name, sizeof(cl_ulong), &value, 0);
    return value;
}

};

