// Copyright Hugh Perkins 2013, 2014, 2015, 2016, 2017 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "CLArrayInt.h"
#include "CLArrayFloat.h"

#include "EasyCL_export.h"

#include "mystdint.h"

namespace easycl {

class CLQueue;

class EasyCL_EXPORT CLKernel {
    EasyCL *cl; // NOT owned by this object, dont delete!
#ifdef _WIN32
#pragma warning(disable: 4251)
#endif
    std::string sourceFilename; // just for info really
    std::string kernelName; // this too
    cl_program program;
    cl_int error;
    std::string source;

    int nextArg;

    std::vector<cl_mem> buffers;

//    std::vector<int> inputArgInts;
//    std::vector<unsigned int> inputArgUInts;
//    std::vector<long long> inputArgLongLongs;
//    std::vector<unsigned long long> inputArgULongLongs;
    std::vector<int32_t> inputArgInt32s;
    std::vector<uint32_t> inputArgUInt32s;
    std::vector<int64_t> inputArgInt64s;
    std::vector<uint64_t> inputArgUInt64s;
    std::vector<float> inputArgFloats;
    std::vector<char> inputArgChars;

    std::vector<cl_mem> outputArgBuffers;
    std::vector<void *> outputArgPointers;
    std::vector<size_t> outputArgSizes;

    std::vector< CLWrapper * > wrappersToDirty; // we will mark these dirty
                                                // on 'run'
                                                // only wrappers passed as `out` or
                                                // or `inout` will be marked dirty
                                                // on run

      template<typename T>
      static std::string toString(T val);
#ifdef _WIN32
#pragma warning(default: 4251)
#endif

public:
    cl_kernel kernel;
    CLKernel(EasyCL *easycl, std::string sourceFilename, std::string kernelName, std::string source, cl_program program, cl_kernel kernel);
    CLKernel(const CLKernel &kernel);
    CLKernel &operator=(const CLKernel &kernel);
    ~CLKernel();

    CLKernel *input(CLArray *clarray1d);
    CLKernel *inout(CLArray *clarray1d);
    CLKernel *output(CLArray *clarray1d);

    // synonyms, in case you prefer `in` instead of `input`:
    CLKernel *in(CLArray *clarray1d) { return input(clarray1d); }
    CLKernel *out(CLArray *clarray1d) { return output(clarray1d); }

    CLKernel *inout(cl_mem *buf);
    CLKernel *in_nullptr();

    CLKernel *input(CLWrapper *wrapper);
    CLKernel *output(CLWrapper *wrapper);
    CLKernel *inout(CLWrapper *wrapper);

    // synonyms, in case you prefer `in` instead of `input`:
    CLKernel *in(CLWrapper *wrapper) { return input(wrapper); }
    CLKernel *out(CLWrapper *wrapper) { return output(wrapper); }

    CLKernel *localFloats(int count);
    CLKernel *localInts(int count);
    CLKernel *local(int N);

    template<typename T> CLKernel *input(int N, const T *data);
    template<typename T> CLKernel *in(int N, const T *data);
//    CLKernel *input(int value);
//    CLKernel *in(int value);
//    CLKernel *input(unsigned int value);
//    CLKernel *in(unsigned int value);
//    CLKernel *input(long long value);
//    CLKernel *in(long long value);
//    CLKernel *input(unsigned long long value);
//    CLKernel *in(unsigned long long value);
    CLKernel *input(float value);
    CLKernel *in(float value);

    CLKernel *in_int64(int64_t value);
    CLKernel *in_int32(int32_t value);
    CLKernel *in_uint64(uint64_t value);
    CLKernel *in_uint32(uint32_t value);
    CLKernel *in_char(char value);
    CLKernel *in_float(float value);

    // these are deprecated:
    CLKernel *input(int32_t value);
    CLKernel *in(int32_t value);
    CLKernel *input(int64_t value);
    CLKernel *in(int64_t value);
    CLKernel *input(uint64_t value);
    CLKernel *in(uint64_t value);
    CLKernel *input(uint32_t value);
    CLKernel *in(uint32_t value);

    template<typename T> CLKernel *output(int N, T *data);
    template<typename T> CLKernel *out(int N, T *data);
    template<typename T> CLKernel *inout(int N, T *data);
    // void run_1d(int global_worksize, int local_worksize);
    // void run(int ND, const size_t *global_ws, const size_t *local_ws);
    // void run_1d(CLQueue *queue, int global_worksize, int local_worksize);
    // void run(CLQueue *queue, int ND, const size_t *global_ws, const size_t *local_ws);
    void run_1d(int global_worksize, int local_worksize, bool fast_read=false);
    void run_1d(cl_command_queue *queue, int global_worksize, int local_worksize, bool fast_read=false);
    void run_1d(CLQueue *queue, int global_worksize, int local_worksize, bool fast_read=false);
    void run(int ND, const size_t *global_ws, const size_t *local_ws, bool fast_read=false);
    void run(cl_command_queue *queue, int ND, const size_t *global_ws, const size_t *local_ws, bool fast_read=false);
    void run(CLQueue *queue, int ND, const size_t *global_ws, const size_t *local_ws, bool fast_read=false);

    std::string buildLog;
};
}
