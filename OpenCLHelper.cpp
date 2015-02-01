// Copyright Hugh Perkins 2013,2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>
#include <stdexcept>
using namespace std;

#include "OpenCLHelper.h"

#include "CLArrayFloat.h"
#include "CLArrayInt.h"
#include "CLIntWrapper.h"
#include "CLFloatWrapperConst.h"
#include "CLWrapper.h"
#include "CLKernel.h"

OpenCLHelper::OpenCLHelper(int gpu ) {
    init(gpu);
}

OpenCLHelper::OpenCLHelper() {
     init(0);
}

OpenCLHelper::OpenCLHelper( cl_platform_id platform_id, cl_device_id device ) {
    queue = 0;
    context = 0;

    bool clpresent = 0 == clewInit();
    if( !clpresent ) {
        throw std::runtime_error("OpenCL library not found");
    }
    this->platform_id = platform_id;
    this->device = device;

    std::cout << "Using " << getPlatformInfoString( platform_id, CL_PLATFORM_VENDOR ) << " platform: " << getPlatformInfoString( platform_id, CL_PLATFORM_NAME ) << std::endl;
//    std::cout << "Using " << getPlatformInfoString( platform_id, CL_PLATFORM_NAME ) << std::endl;
    std::cout << "Using device: " << getDeviceInfoString( device, CL_DEVICE_NAME ) << std::endl;

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

OpenCLHelper *OpenCLHelper::createForFirstGpu() {
    return createForIndexedGpu( 0 );
}

OpenCLHelper *OpenCLHelper::createForFirstGpuOtherwiseCpu() {
    try {
        return createForIndexedGpu( 0 );
    } catch( std::runtime_error error ) {
        cout << "Couldnt find available GPU: " << error.what() << endl;
        cout << "Trying for CPU" << endl;
    }
    return createForPlatformDeviceIndexes( 0, 0 );
}

OpenCLHelper *OpenCLHelper::createForIndexedGpu( int gpu ) {
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
//        cout << "checking platform id " << platform_id << endl;
        cl_device_id device_ids[100];
        cl_uint num_devices;
        error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR , 100, device_ids, &num_devices);
        if (error != CL_SUCCESS) {
            continue;
//           throw std::runtime_error( "Error getting device ids for platform " + toString( platform ) + ": " + errorMessage(error) );
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

OpenCLHelper *OpenCLHelper::createForPlatformDeviceIndexes(int platformIndex, int deviceIndex) {
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

OpenCLHelper *OpenCLHelper::createForPlatformDeviceIds(cl_platform_id platformId, cl_device_id deviceId) {
    return new OpenCLHelper( platformId, deviceId );
}

void OpenCLHelper::init(int gpuIndex ) {
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

OpenCLHelper::~OpenCLHelper() {
//        clReleaseProgram(program);
    if( queue != 0 ) {
//        cout << "releasing OpenCL command queue" << endl;
        clReleaseCommandQueue(*queue);
        delete queue;
    }
    if( context != 0 ) {
//        cout << "releasing OpenCL context" << endl;
        clReleaseContext(*context);        
        delete context;
    }
}

CLArrayFloat *OpenCLHelper::arrayFloat(int N ) {
    return new CLArrayFloat( N, this );
}

CLArrayInt *OpenCLHelper::arrayInt(int N ) {
    return new CLArrayInt( N, this );
}

CLIntWrapper *OpenCLHelper::wrap(int N, int *source ) {
    return new CLIntWrapper( N, source, this );
}

CLFloatWrapper *OpenCLHelper::wrap(int N, float *source ) {
    return new CLFloatWrapper( N, source, this );
}

CLFloatWrapperConst *OpenCLHelper::wrap(int N, float const*source ) {
    return new CLFloatWrapperConst( N, source, this );
}

CLKernel *OpenCLHelper::buildKernel( string kernelfilepath, string kernelname ) {
    return buildKernel( kernelfilepath, kernelname, "" );
}

CLKernel *OpenCLHelper::buildKernel( string kernelfilepath, string kernelname, string options ) {
    std::string path = kernelfilepath.c_str();
    std::string source = getFileContents(path);
    return buildKernelFromString( source, kernelname, options, kernelfilepath );
}

CLKernel *OpenCLHelper::buildKernelFromString( string source, string kernelname, string options, string sourcefilename ) {
    size_t src_size = 0;
    const char *source_char = source.c_str();
    src_size = strlen( source_char );
//    cl_program program = new cl_program();
    cl_program program = clCreateProgramWithSource(*context, 1, &source_char, &src_size, &error);
    checkError(error);

//    error = clBuildProgram(program, 1, &device, "-cl-opt-disable", NULL, NULL);
//    std::cout << "options: [" << options.c_str() << "]" << std::endl;
    error = clBuildProgram(program, 1, &device, options.c_str(), NULL, NULL);

    char* build_log;
    size_t log_size;
    error = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    checkError(error);
    build_log = new char[log_size+1];
    error = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    checkError(error);
    build_log[log_size] = '\0';
    string buildLogMessage = "";
    if( log_size > 2 ) {
        buildLogMessage = sourcefilename + " build log: "  + "\n" + build_log;
        cout << buildLogMessage << endl;
    }
    delete[] build_log;
    checkError(error);

    cl_kernel kernel = clCreateKernel(program, kernelname.c_str(), &error);
    switch( error ) {
        case CL_SUCCESS:
            break;
        case -46:
            throw std::runtime_error( "Invalid kernel name, code -46, kernel " + kernelname + "\n" + buildLogMessage );
            break;
        default:
            throw std::runtime_error( "Something went wrong with clCreateKernel, code " + toString( error ) + "\n" + buildLogMessage );
            break;
    }
    checkError(error);
//    clReleaseProgram(program);
    return new CLKernel(this, program, kernel);
}

bool OpenCLHelper::isOpenCLAvailable() {
    return 0 == clewInit();
}

int OpenCLHelper::getPower2Upperbound( int value ) {
    int upperbound = 1;
    while( upperbound < value ) {
        upperbound <<= 1;
    }
    return upperbound;
}

int OpenCLHelper::roundUp( int quantization, int minimum ) {
    int size = ( minimum / quantization) * quantization;
    if( size < minimum ) {
        size += quantization;
    }
    return size;
}

// accidentally created 2 funcftions that do the same thing :-P  but wont remove either,
// in case someone's using that one
int OpenCLHelper::getNextPower2( int value ){ 
    return getPower2Upperbound( value ); 
} // eg pass in 320, it will return: 512

void OpenCLHelper::gpu( int gpuIndex ) {
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

void OpenCLHelper::finish() {
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

int OpenCLHelper::getComputeUnits() {
    return (int)getDeviceInfoInt(CL_DEVICE_MAX_COMPUTE_UNITS);
}

int OpenCLHelper::getLocalMemorySize() {
    return (int)getDeviceInfoInt(CL_DEVICE_LOCAL_MEM_SIZE);
}

int OpenCLHelper::getMaxWorkgroupSize() {
    return (int)getDeviceInfoInt(CL_DEVICE_MAX_WORK_GROUP_SIZE);
}

std::string OpenCLHelper::errorMessage(cl_int error ) {
    return toString(error);
}

void OpenCLHelper::checkError( cl_int error ) {
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

std::string OpenCLHelper::getFileContents( std::string filename ) {
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

long OpenCLHelper::getDeviceInfoInt( cl_device_info name ) {
    cl_ulong value = 0;
    clGetDeviceInfo(device, name, sizeof(cl_ulong), &value, 0);
    return static_cast<long>( value );
}


