// Copyright Hugh Perkins 2013, 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "clew.h"

class OpenCLHelper {
public:
     cl_int error;  

    cl_device_id *device_ids;

    cl_uint num_platforms;
    cl_uint num_devices;

    cl_platform_id platform_id;
    cl_device_id device;

    cl_context *context;
    cl_command_queue *queue;
//    cl_program *program;

    int gpuIndex;

    class CLArrayFloat *arrayFloat(int N );
    class CLArrayInt *arrayInt(int N );
    class CLIntWrapper *wrap(int N, int *source );
    class CLFloatWrapper *wrap(int N, float *source );
    class CLFloatWrapperConst *wrap(int N, float const*source );

    static bool isOpenCLAvailable() {
        return 0 == clewInit();
    }

    ~OpenCLHelper() {
//        clReleaseProgram(program);
        if( queue != 0 ) {
            clReleaseCommandQueue(*queue);
            delete queue;
        }
        if( context != 0 ) {
            clReleaseContext(*context);        
            delete context;
        }
    }

    static int roundUp( int quantization, int minimum ) {
        int size = ( minimum / quantization) * quantization;
        if( size < minimum ) {
            size += quantization;
        }
        return size;
    }

    static int getNextPower2( int value ) { // eg pass in 320, it will return: 512
        int power2 = 0;
        while( value > 0 ) {
            value >>= 1;
            power2++;
        }
        return 1 << power2;
    }

    static int getPower2Upperbound( int value ) {
        int upperbound = 1;
        while( upperbound < value ) {
            upperbound <<= 1;
        }
        return upperbound;
    }

    void gpu( int gpuIndex ) {
        if( gpuIndex >= num_devices ) {
           throw std::runtime_error( "requested gpuindex " + toString( gpuIndex ) + " goes beyond number of available device " + toString( num_devices ) );
        }
        device = device_ids[gpuIndex];

        if( queue != 0 ) {
            clReleaseCommandQueue(*queue);
            delete queue;
        }
        if( context != 0 ) {
            clReleaseContext(*context);        
            delete context;
        }

        // Context
        context = new cl_context();
        *context = clCreateContext(0, 1, &device, NULL, NULL, &error);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error creating context: " + errorMessage(error) );
        }
        // Command-queue
        queue = new cl_command_queue;
        *queue = clCreateCommandQueue(*context, device, 0, &error);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error creating command queue: " + errorMessage(error) );
        }
    }

    OpenCLHelper(int gpu) {
        init(gpu);
    }

    OpenCLHelper() {
         init(0);
    }

    void init(int gpuindex) {
        bool clpresent = 0 == clewInit();
        if( !clpresent ) {
            throw std::runtime_error("OpenCL library not found");
        }

//        std::cout << "this: " << this << std::endl;
        this->gpuIndex = gpuindex;
        error = 0;

        queue = 0;
        context = 0;

        // Platform
        error = clGetPlatformIDs(1, &platform_id, &num_platforms);
//        std::cout << "num platforms: " << num_platforms << std::endl;
//        assert (num_platforms == 1);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting platforms ids: " + errorMessage(error) );
        }
        if( num_platforms == 0 ) {
           throw std::runtime_error( "Error: no platforms available" );
        }

        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting device ids: " + errorMessage(error) );
        }
  //      std::cout << "num devices: " << num_devices << std::endl;
        device_ids = new cl_device_id[num_devices];
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, num_devices, device_ids, &num_devices);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting device ids: " + errorMessage(error) );
        }

        gpu( gpuindex );
    }

    void finish() {
        error = clFinish( *queue );
        switch( error ) {
            case CL_SUCCESS:
                break;
            case -36:
                throw std::runtime_error( "Invalid command queue: often indicates out of bounds memory access within kernel" );
            default:
                checkError(error);                
        }
    }

    class CLKernel *buildKernel( std::string kernelfilepath, std::string kernelname );
    class CLKernel *buildKernel( std::string kernelfilepath, std::string kernelname, std::string options );
    class CLKernel *buildKernelFromString( std::string source, std::string kernelname, std::string options );

    int getComputeUnits() {
        return (int)getDeviceInfoInt(CL_DEVICE_MAX_COMPUTE_UNITS);
    }

    int getLocalMemorySize() {
        return (int)getDeviceInfoInt(CL_DEVICE_LOCAL_MEM_SIZE);
    }

    int getMaxWorkgroupSize() {
        return (int)getDeviceInfoInt(CL_DEVICE_MAX_WORK_GROUP_SIZE);
    }



static std::string errorMessage(cl_int error ) {
    return toString(error);
}

static void checkError( cl_int error ) {
    if( error != CL_SUCCESS ) {
        std::string message = toString(error);
        switch( error ) {
            case CL_INVALID_ARG_SIZE:
                message = "CL_INVALID_ARG_SIZE";
                break;
            case CL_INVALID_BUFFER_SIZE:
                message = "CL_INVALID_BUFFER_SIZE";
                break;
        }
        throw std::runtime_error( std::string("error: ") + message );
    }
}

private:


static std::string getFileContents( std::string filename ) {
    char * buffer = 0;
    long length;
    FILE * f = fopen (filename.c_str(), "rb");

    std::string returnstring = "";
    if (f)
    {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = new char[length + 1];
      if (buffer) {
        int bytesread = fread (buffer, 1, length, f);
        if( bytesread != length ) {
            throw std::runtime_error( "Failed to read cl source file" );
        }
      } else {
        throw std::runtime_error( "Failed to allocate memory for cl source" );
       }
      fclose (f);
        buffer[length] = 0;
      returnstring = std::string( buffer );
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

#include "CLIntWrapper.h"
#include "CLFloatWrapper.h"
#include "CLFloatWrapperConst.h"
#include "CLWrapper.h"
#include "CLKernel.h"


