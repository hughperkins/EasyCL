// Copyright Hugh Perkins 2013, 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "clew.h"

#include "deprecated.h"

#include "OpenCLHelper_export.h"

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

class OpenCLHelper_EXPORT OpenCLHelper {
public:
     cl_int error;  // easier than constantly declaring it in each method...

//    cl_device_id *device_ids;

//    cl_uint num_platforms;
//    cl_uint num_devices;

    cl_platform_id platform_id;
    cl_device_id device;

    cl_context *context;
    cl_command_queue *queue;
//    cl_program *program;

//    int gpuIndex;

    class CLArrayFloat *arrayFloat(int N );
    class CLArrayInt *arrayInt(int N );
    class CLIntWrapper *wrap(int N, int *source );
    class CLFloatWrapper *wrap(int N, float *source );
    class CLFloatWrapperConst *wrap(int N, float const*source );

    static bool isOpenCLAvailable() {
        return 0 == clewInit();
    }

    template<typename T>
    static std::string toString(T val ) {
       std::ostringstream myostringstream;
       myostringstream << val;
       return myostringstream.str();
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

    // accidentally created 2 funcftions that do the same thing :-P  but wont remove either,
    // in case someone's using that one
    static int getNextPower2( int value ){ return getPower2Upperbound( value ); } // eg pass in 320, it will return: 512
    static int getPower2Upperbound( int value );

    void gpu( int gpuIndex ) {
        if( queue != 0 ) {
            clReleaseCommandQueue(*queue);
            delete queue;
        }
        if( context != 0 ) {
            clReleaseContext(*context);        
            delete context;
        }

        init( gpuIndex );
    }

    OpenCLHelper(int gpu ) {
        init(gpu);
    }

    OpenCLHelper() {
         init(0);
    }

    OpenCLHelper( cl_platform_id platformId, cl_device_id deviceId ) {
        bool clpresent = 0 == clewInit();
        if( !clpresent ) {
            throw std::runtime_error("OpenCL library not found");
        }
        this->platform_id = platform_id;
        this->device = deviceId;
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

    static OpenCLHelper *createForFirstGpu() {
        return createForIndexedGpu( 0 );
    }

    static OpenCLHelper *createForFirstGpuOtherwiseCpu() {
        try {
            return createForIndexedGpu( 0 );
        } catch( std::runtime_error error ) {
        }
        return createForPlatformDeviceIndexes( 0, 0 );
    }

    static OpenCLHelper *createForIndexedGpu( int gpu ) {
        bool clpresent = 0 == clewInit();
        if( !clpresent ) {
            throw std::runtime_error("OpenCL library not found");
        }
        cl_int error;
        int currentGpuIndex = 0;
        cl_platform_id platform_ids[10];
        cl_uint num_platforms;
        error = clGetPlatformIDs(10, platform_ids, &num_platforms);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting platforms ids: " + errorMessage(error) );
        }
        if( num_platforms == 0 ) {
           throw std::runtime_error( "Error: no platforms available" );
        }
        for( int platform =  0; platform < (int)num_platforms; platform++ ) {
            cl_platform_id platform_id = platform_ids[platform];
            cl_device_id device_ids[100];
            cl_uint num_devices;
            error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR , 100, device_ids, &num_devices);
            if (error != CL_SUCCESS) {
               throw std::runtime_error( "Error getting device ids for platform " + toString( platform ) + ": " + errorMessage(error) );
            }
            if( ( gpu - currentGpuIndex ) < (int)num_devices ) {
                return new OpenCLHelper( platform_id, device_ids[( gpu - currentGpuIndex )] );
            } else {
                currentGpuIndex += num_devices;
            }
        }
        if( gpu == 0 ) {
            throw std::runtime_error("No gpus found" );
        } else {
            throw std::runtime_error("Not enough gpus found to satisfy gpu index: " + toString( gpu ) );
        }
    }

    static OpenCLHelper *createForPlatformDeviceIndexes(int platformIndex, int deviceIndex) {
        bool clpresent = 0 == clewInit();
        if( !clpresent ) {
            throw std::runtime_error("OpenCL library not found");
        }
        cl_int error;
        int currentGpuIndex = 0;
        cl_platform_id platform_ids[10];
        cl_uint num_platforms;
        error = clGetPlatformIDs(10, platform_ids, &num_platforms);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting platforms ids: " + errorMessage(error) );
        }
        if( num_platforms == 0 ) {
           throw std::runtime_error( "Error: no platforms available" );
        }
        if( platformIndex >= (int)num_platforms ) {
           throw std::runtime_error( "Error: platform index " + toString( platformIndex ) + " not available. There are only: " + toString( num_platforms ) + " platforms available" );
        }
        cl_platform_id platform_id = platform_ids[platformIndex];
        cl_device_id device_ids[100];
        cl_uint num_devices;
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 100, device_ids, &num_devices);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting device ids for platform index " + toString( platformIndex ) + ": " + errorMessage(error) );
        }
        if( num_devices == 0 ) {
           throw std::runtime_error( "Error: no devices available for platform index " + toString( platformIndex ) );
        }
        if( deviceIndex >= (int)num_devices ) {
           throw std::runtime_error( "Error: device index " + toString(deviceIndex) + " goes beyond the available devices on platform index " + toString( platformIndex ) + ", which has " + toString( num_devices ) + " devices" );
        }
        return new OpenCLHelper( platform_id, device_ids[deviceIndex] );
    }

    static OpenCLHelper *createForPlatformDeviceIds(cl_platform_id platformId, cl_device_id deviceId) {
        return new OpenCLHelper( platformId, deviceId );
    }

    void init(int gpuIndex ) {
        bool clpresent = 0 == clewInit();
        if( !clpresent ) {
            throw std::runtime_error("OpenCL library not found");
        }

//        std::cout << "this: " << this << std::endl;
//        this->gpuIndex = gpuindex;
        error = 0;

        queue = 0;
        context = 0;

        // Platform
        cl_uint num_platforms;
        error = clGetPlatformIDs(1, &platform_id, &num_platforms);
//        std::cout << "num platforms: " << num_platforms << std::endl;
//        assert (num_platforms == 1);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting platforms ids: " + errorMessage(error) );
        }
        if( num_platforms == 0 ) {
           throw std::runtime_error( "Error: no platforms available" );
        }

        cl_uint num_devices;
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR , 1, &device, &num_devices);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting device ids: " + errorMessage(error) );
        }
  //      std::cout << "num devices: " << num_devices << std::endl;
        cl_device_id *device_ids = new cl_device_id[num_devices];
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR, num_devices, device_ids, &num_devices);
        if (error != CL_SUCCESS) {
           throw std::runtime_error( "Error getting device ids: " + errorMessage(error) );
        }

        if( gpuIndex >= static_cast<int>( num_devices ) ) {
           throw std::runtime_error( "requested gpuindex " + toString( gpuIndex ) + " goes beyond number of available device " + toString( num_devices ) );
        }
        device = device_ids[gpuIndex];
        delete[] device_ids;

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
    class CLKernel *buildKernelFromString( std::string source, std::string kernelname, std::string options, std::string sourcefilename = "" );

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
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

long getDeviceInfoInt( cl_device_info name ) {
    cl_ulong value = 0;
    clGetDeviceInfo(device, name, sizeof(cl_ulong), &value, 0);
    return static_cast<long>( value );
}

};

#include "CLIntWrapper.h"
#include "CLFloatWrapper.h"
#include "CLFloatWrapperConst.h"
#include "CLWrapper.h"
#include "CLKernel.h"


