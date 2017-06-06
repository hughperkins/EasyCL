// Copyright Hugh Perkins 2013, 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#ifdef USE_CLEW
#include "clew.h"
#else
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#endif

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <map>

#include "deviceinfo_helper.h"
#include "platforminfo_helper.h"

#include "deprecated.h"

#include "EasyCL_export.h"

namespace easycl {
class CLKernel;
class CLArrayFloat;
class CLArrayInt;
class CLIntWrapper;
class CLFloatWrapper;
class CLFloatWrapperConst;
class CLUCharWrapper;

class EasyCL;

class EasyCL_EXPORT CLQueue {
public:
    CLQueue(EasyCL *cl); // creates new command queue, releases it at the end
    CLQueue(EasyCL *cl, cl_command_queue queue); // takes repsonsibilty for this queue; releases it at the end
    ~CLQueue();

    EasyCL *cl;
    cl_command_queue queue;
};

class EasyCL_EXPORT EasyCL {
public:
    bool verbose;

     cl_int error;  // easier than constantly declaring it in each method...

    cl_platform_id platform_id;
    cl_device_id device;

    cl_context *context;
    cl_command_queue *queue;
    CLQueue *default_queue;  // contains the queue in `queue` above (we dont delete this CLQueue object ever though)

    bool profilingOn;

    template<typename T>
    static std::string toString(T val) {
       std::ostringstream myostringstream;
       myostringstream << val;
       return myostringstream.str();
    }

    void commonConstructor(cl_platform_id platform_id, cl_device_id device, bool verbose);
    EasyCL(int gpu);
    EasyCL();
    EasyCL(int gpu, bool verbose);
    EasyCL(bool verbose);
    EasyCL(cl_platform_id platformId, cl_device_id deviceId);
    EasyCL(cl_platform_id platformId, cl_device_id deviceId, bool verbose);
    virtual ~EasyCL();

    static bool isOpenCLAvailable();
    static int roundUp(int quantization, int minimum);
    // accidentally created 2 funcftions that do the same thing :-P  but wont remove either,
    // in case someone's using that one
    static int getNextPower2(int value); // eg pass in 320, it will return: 512
    static int getPower2Upperbound(int value);
    static EasyCL *createForFirstGpu();
    static EasyCL *createForFirstGpuOtherwiseCpu();
    static EasyCL *createForIndexedDevice(int device);
    static EasyCL *createForIndexedGpu(int gpu);
    static EasyCL *createForFirstGpu(bool verbose);
    static EasyCL *createForFirstGpuOtherwiseCpu(bool verbose);
    static EasyCL *createForIndexedGpu(int gpu, bool verbose);
    static EasyCL *createForIndexedDevice(int device, bool verbose);
    static EasyCL *createForPlatformDeviceIndexes(int platformIndex, int deviceIndex);
    static EasyCL *createForPlatformDeviceIds(cl_platform_id platformId, cl_device_id deviceId);
    static std::string errorMessage(cl_int error);
    static void checkError(cl_int error);

    void gpu(int gpuIndex);
    void init(int gpuIndex, bool verbose);
    void finish();

    void setProfiling(bool profiling);
    void pushEvent(std::string name, cl_event *event);
    void dumpProfiling();

    int getComputeUnits();
    int getLocalMemorySize();
    int getLocalMemorySizeKB();
    int getMaxWorkgroupSize();
    int getMaxAllocSizeMB();

    CLQueue *newQueue(); // you own this, should delete it
    // void destroyQueue()

    CLArrayFloat *arrayFloat(int N);
    CLArrayInt *arrayInt(int N);
    CLIntWrapper *wrap(int N, int *source);
    CLUCharWrapper *wrap(int N, unsigned char *source);
    CLFloatWrapper *wrap(int N, float *source);
    CLFloatWrapperConst *wrap(int N, float const*source);
    CLKernel *buildKernel(std::string kernelfilepath, std::string kernelname, bool quiet=false);
    CLKernel *buildKernel(std::string kernelfilepath, std::string kernelname, std::string options, bool quiet=false);
    CLKernel *buildKernelFromString(std::string source, std::string kernelname, std::string options, std::string sourcefilename = "", bool quiet=false);

    // simple associate-array of kernels, specific to each EasyCL object
    // so we can cache them easily, if we want
    // good to make the cache per-connection, ie per-EasyCL object
    // so here is not a bad place to put this?
    void storeKernel(std::string name, CLKernel *kernel); // throws exception if name already assigned
    void storeKernel(std::string name, CLKernel *kernel, bool deleteWithCl); // if deleteWithCl true, then
                                // when the EasyCL object is deleted, so will be the kernel
    CLKernel *getKernel(std::string name);  // throw exception if name doesnt exist
    bool kernelExists(std::string name);
private:
// remove the warnings about dll-interface for kernelByName, which we 
// can safely ignore, since kernelByName is private
#ifdef _WIN32
#pragma warning(disable: 4251)
#endif
    std::map< std::string, CLKernel * >kernelByName;
    std::map< std::string, bool >kernelOwnedByName; // should we delete the kernel when we are deleted?
    std::vector< cl_event *> profilingEvents;
    std::vector< std::string > profilingNames;
#ifdef _WIN32
#pragma warning(default: 4251)
#endif

    static std::string getFileContents(std::string filename);
//    long getDeviceInfoInt(cl_device_info name);
    int64_t getDeviceInfoInt64(cl_device_info name);
};
}

#include "CLIntWrapper.h"
#include "CLFloatWrapper.h"
#include "CLFloatWrapperConst.h"
#include "CLUCharWrapper.h"
#include "CLWrapper.h"
#include "CLKernel.h"
#include "DevicesInfo.h"
