// Copyright Hugh Perkins 2013, 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <stdexcept>
using namespace std;

#include "CLKernel.h"
#include "CLArrayFloat.h"
#include "CLArrayInt.h"
#include "CLArray.h"
#include "util/easycl_stringhelper.h"

#include "EasyCL_export.h"

template<typename T>
std::string CLKernel::toString(T val) {
	std::ostringstream myostringstream;
	myostringstream << val;
	return myostringstream.str();
}

CLKernel::CLKernel(EasyCL *easycl, std::string sourceFilename, std::string kernelName, std::string source, cl_program program, cl_kernel kernel) {
  this->sourceFilename = sourceFilename;
  this->kernelName = kernelName;
  this->source = source;
	this->easycl = easycl;
	nextArg = 0;
	error = CL_SUCCESS;
	this->program = program;
	this->kernel = kernel;
}
CLKernel::CLKernel(const CLKernel &kernel) {
	throw std::runtime_error("can't assign CLKernel");
}
CLKernel &CLKernel::operator=(const CLKernel &kernel) {
	throw std::runtime_error("can't assign CLKernel");
}
CLKernel::~CLKernel() {
	clReleaseProgram(program);
	clReleaseKernel(kernel);
}

CLKernel *CLKernel::input(CLArray *clarray1d) {
    assert(clarray1d != 0);
    if(!clarray1d->isOnDevice()) {
        clarray1d->moveToDevice();
    }
    if(clarray1d->isOnHost()) {
        clarray1d->deleteFromHost();
    }
    cl_mem *devicearray = clarray1d->getDeviceArray();
    error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), devicearray);
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::output(CLArray *clarray1d) {
    assert(clarray1d != 0);
    if(clarray1d->isOnHost()) {
        clarray1d->deleteFromHost();
    }
    if(!clarray1d->isOnDevice()) {
        clarray1d->createOnDevice();
    }
    assert(clarray1d->isOnDevice() && !clarray1d->isOnHost());
    error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), (clarray1d->getDeviceArray()));
    easycl->checkError(error);
    nextArg++;  
    return this;      
}

CLKernel *CLKernel::inout(cl_mem *buf) {
    // assert(clarray1d != 0);
    // if(!clarray1d->isOnDevice()) {
    //     clarray1d->moveToDevice();
    // }
    // if(clarray1d->isOnHost()) {
    //     clarray1d->deleteFromHost();
    // }
    // cl_mem *devicearray = clarray1d->getDeviceArray();
    error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), buf);
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::inout(CLArray *clarray1d) {
    assert(clarray1d != 0);
    if(!clarray1d->isOnDevice()) {
        clarray1d->moveToDevice();
    }
    if(clarray1d->isOnHost()) {
        clarray1d->deleteFromHost();
    }
    cl_mem *devicearray = clarray1d->getDeviceArray();
    error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), devicearray);
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::input(CLWrapper *wrapper) {
    assert(wrapper != 0);
    if(!wrapper->isOnDevice()) {
        throw std::runtime_error("need to copyToDevice() before calling kernel->input");
    }
    cl_mem *devicearray = wrapper->getDeviceArray();
    error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), devicearray);
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::inout(CLWrapper *wrapper) {
    assert(wrapper != 0);
    if(!wrapper->isOnDevice()) {
        throw std::runtime_error("need to copyToDevice() before calling kernel->input");
    }
    cl_mem *devicearray = wrapper->getDeviceArray();
    error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), devicearray);
    easycl->checkError(error);
    nextArg++;
    wrappersToDirty.push_back(wrapper);
    return this;
}

CLKernel *CLKernel::output(CLWrapper *wrapper) {
    assert(wrapper != 0);
    if(!wrapper->isOnDevice()) {
        wrapper->createOnDevice();
    }
    error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), (wrapper->getDeviceArray()));
    easycl->checkError(error);
    nextArg++;
    wrappersToDirty.push_back(wrapper);
    return this;      
}

CLKernel *CLKernel::localFloats(int count) {
	error = clSetKernelArg(kernel, nextArg, count * sizeof(cl_float), 0);
	easycl->checkError(error);
	nextArg++;
	return this;
}
CLKernel *CLKernel::localInts(int count) {
	error = clSetKernelArg(kernel, nextArg, count * sizeof(cl_int), 0);
	easycl->checkError(error);
	nextArg++;
	return this;
}
CLKernel *CLKernel::local(int N) {
	return localFloats(N);
}
#define CLKERNEL_CREATE_SCALAR_INPUT(TYPE, NAME) \
CLKernel *CLKernel::input(TYPE value) { \
	inputArg##NAME##s.push_back(value); \
	error = clSetKernelArg(kernel, nextArg, sizeof(TYPE), &(inputArg##NAME##s[inputArg##NAME##s.size() - 1])); \
	easycl->checkError(error); \
	nextArg++; \
	return this; \
} \
CLKernel *CLKernel::in(TYPE value) { \
	return input(value); \
}
//CLKERNEL_CREATE_SCALAR_INPUT(int, Int);
//CLKERNEL_CREATE_SCALAR_INPUT(unsigned int, UInt);
//CLKERNEL_CREATE_SCALAR_INPUT(long long, LongLong);
//CLKERNEL_CREATE_SCALAR_INPUT(unsigned long long, ULongLong);
CLKERNEL_CREATE_SCALAR_INPUT(int32_t, Int32);
CLKERNEL_CREATE_SCALAR_INPUT(uint32_t, UInt32);
CLKERNEL_CREATE_SCALAR_INPUT(int64_t, Int64);
CLKERNEL_CREATE_SCALAR_INPUT(uint64_t, UInt64);

CLKERNEL_CREATE_SCALAR_INPUT(float, Float);

//CLKernel *CLKernel::input(unsigned int value) {
//	inputArgUInts.push_back(value);
//	error = clSetKernelArg(kernel, nextArg, sizeof(unsigned int), &(inputArgUInts[inputArgUInts.size() - 1]));
//	easycl->checkError(error);
//	nextArg++;
//	return this;
//}
//CLKernel *CLKernel::input(long value) {
//	inputArgInt64s.push_back(value);
//	error = clSetKernelArg(kernel, nextArg, sizeof(long), &(inputArgInt64s[inputArgInt64s.size() - 1]));
//	easycl->checkError(error);
//	nextArg++;
//	return this;
//}
//CLKernel *CLKernel::input(float value) {
//	inputArgFloats.push_back(value);
//	error = clSetKernelArg(kernel, nextArg, sizeof(float), &(inputArgFloats[inputArgFloats.size() - 1]));
//	easycl->checkError(error);
//	nextArg++;
//	return this;
//}
//CLKernel *CLKernel::in(float value) {
//	return input(value);
//}

#ifndef _CLKERNEL_STRUCTS_H
template<typename T> CLKernel *CLKernel::input(int N, const T *data) {
	cl_mem buffer = clCreateBuffer(*(easycl->context), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(T) * N, (void *)data, &error);
	easycl->checkError(error);
	error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
	easycl->checkError(error);
	buffers.push_back(buffer);
	nextArg++;
	return this;
}
template<typename T>
CLKernel *CLKernel::in(int N, const T *data) {
	return input(N, data);
}
template<typename T>
CLKernel *CLKernel::output(int N, T *data) {
	cl_mem buffer = clCreateBuffer(*(easycl->context), CL_MEM_WRITE_ONLY, sizeof(T) * N, 0, &error);
	easycl->checkError(error);
	error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
	buffers.push_back(buffer);
	//outputArgNums.push_back(nextArg);
	outputArgBuffers.push_back(buffer);
	outputArgPointers.push_back((void *)data);
	outputArgSizes.push_back(sizeof(T) * N);
	nextArg++;
	return this;
}
template<typename T>
CLKernel *CLKernel::out(int N, T *data) {
	return output(N, data);
}
template<typename T>
CLKernel *CLKernel::inout(int N, T *data) {
	cl_mem buffer = clCreateBuffer(*(easycl->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(T) * N, (void *)data, &error);
	easycl->checkError(error);
	error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
	easycl->checkError(error);
	buffers.push_back(buffer);
	outputArgBuffers.push_back(buffer);
	outputArgPointers.push_back((void *)(data) );
	outputArgSizes.push_back(sizeof(T) * N);
	nextArg++;
	return this;
}
#endif // _CLKERNEL_STRUCTS_H

void CLKernel::run_1d(int global_worksize, int local_worksize) {
	// size_t global_ws = global_worksize;
	// size_t local_ws = local_worksize;
	run_1d(easycl->queue, global_worksize, local_worksize);
}

void CLKernel::run_1d(CLQueue *clqueue, int global_worksize, int local_worksize) {
    // size_t global_ws = global_worksize;
    // size_t local_ws = local_worksize;
    run_1d(&clqueue->queue, global_worksize, local_worksize);
}

void CLKernel::run(int ND, const size_t *global_ws, const size_t *local_ws) {
    run(easycl->queue, ND, global_ws, local_ws);
}

void CLKernel::run(CLQueue *clqueue, int ND, const size_t *global_ws, const size_t *local_ws) {
    run(&clqueue->queue, ND, global_ws, local_ws);
}

void CLKernel::run_1d(cl_command_queue *queue, int global_worksize, int local_worksize) {
    size_t global_ws = global_worksize;
    size_t local_ws = local_worksize;
    run(queue, 1, &global_ws, &local_ws);
}

void CLKernel::run(cl_command_queue *queue, int ND, const size_t *global_ws, const size_t *local_ws) {
	//cout << "running kernel" << std::endl;
  cl_event *event = 0;
  if(easycl->profilingOn) {
    event = new cl_event();
    easycl->pushEvent(sourceFilename + "." + kernelName, event);
  }
	error = clEnqueueNDRangeKernel(*(queue), kernel, ND, NULL, global_ws, local_ws, 0, NULL, event);
  if(error != 0) {
      vector<std::string> splitSource = easycl::split(source, "\n");
      std::string sourceWithNumbers = "\nkernel source:\n";
      for(int i = 0; i < (int)splitSource.size(); i++) {
          sourceWithNumbers += toString(i + 1) + ": " + splitSource[i] + "\n";
      }
	    switch (error) {
	        case -4:
		        throw std::runtime_error(sourceWithNumbers + "\nMemory object allocation failure, code -4");
		        break;
	        case -5:
		        throw std::runtime_error(sourceWithNumbers + "\nOut of resources, code -5");
		        break;
	        case -11:
		        throw std::runtime_error(sourceWithNumbers + "\nProgram build failure, code -11");
		        break;
	        case -46:
		        throw std::runtime_error(sourceWithNumbers + "\nInvalid kernel name, code -46");
		        break;
	        case -52:
		        throw std::runtime_error(sourceWithNumbers + "\nInvalid kernel args, code -52");
		        break;
	        case -54:
		        throw std::runtime_error(sourceWithNumbers + "\nInvalid work group size, code -54");
		        break;
	        default:
		        throw std::runtime_error(sourceWithNumbers + "\nSomething went wrong, code " + toString(error));
      }
	}
	easycl->checkError(error);
	//        error = clFinish(easycl->queue);
	//        easycl->checkError(error);
	//}

	//void retrieveresultsandcleanup() {
	for (int i = 0; i < (int)outputArgBuffers.size(); i++) {
		clEnqueueReadBuffer(*(queue), outputArgBuffers[i], CL_TRUE, 0, outputArgSizes[i], outputArgPointers[i], 0, NULL, NULL);
	}
	//        std::cout << "done" << std::endl;

	for (int i = 0; i < (int)buffers.size(); i++) {
		clReleaseMemObject(buffers[i]);
	}
    // mark wrappers dirty:
    for(int i = 0; i < (int)wrappersToDirty.size(); i++) {
        wrappersToDirty[i]->markDeviceDirty();
    }
	buffers.clear();
	outputArgBuffers.clear();
	outputArgPointers.clear();
	outputArgSizes.clear();
	inputArgInt32s.clear();
	inputArgUInt32s.clear();
	inputArgInt64s.clear();
	inputArgUInt64s.clear();
	inputArgFloats.clear();
    wrappersToDirty.clear();
	nextArg = 0;
}

// template class std::vector<cl_mem>;

#define EASYCL_INSTANTIATE_FOR_TYPE(TYPE) \
template EasyCL_EXPORT CLKernel *CLKernel::input(int N, const TYPE *data); \
template EasyCL_EXPORT CLKernel *CLKernel::in(int N, const TYPE *data); \
template EasyCL_EXPORT CLKernel *CLKernel::output(int N, TYPE *data); \
template EasyCL_EXPORT CLKernel *CLKernel::out(int N, TYPE *data); \
template EasyCL_EXPORT CLKernel *CLKernel::inout(int N, TYPE *data);

EASYCL_INSTANTIATE_FOR_TYPE(float);

//EASYCL_INSTANTIATE_FOR_TYPE(int);
//EASYCL_INSTANTIATE_FOR_TYPE(unsigned int);
//EASYCL_INSTANTIATE_FOR_TYPE(long long);
//EASYCL_INSTANTIATE_FOR_TYPE(unsigned long long);
EASYCL_INSTANTIATE_FOR_TYPE(int32_t);
EASYCL_INSTANTIATE_FOR_TYPE(uint32_t);
EASYCL_INSTANTIATE_FOR_TYPE(int64_t);
EASYCL_INSTANTIATE_FOR_TYPE(uint64_t);

//template EasyCL_EXPORT CLKernel *CLKernel::input(int N, const float *data);
//template EasyCL_EXPORT CLKernel *CLKernel::input(int N, const int *data);
//template EasyCL_EXPORT CLKernel *CLKernel::in(int N, const float *data);
//template EasyCL_EXPORT CLKernel *CLKernel::in(int N, const int *data);
//template EasyCL_EXPORT CLKernel *CLKernel::output(int N, float *data);
//template EasyCL_EXPORT CLKernel *CLKernel::output(int N, int *data);
//template EasyCL_EXPORT CLKernel *CLKernel::out(int N, float *data);
//template EasyCL_EXPORT CLKernel *CLKernel::out(int N, int *data);
//template EasyCL_EXPORT CLKernel *CLKernel::inout(int N, float *data);
//template EasyCL_EXPORT CLKernel *CLKernel::inout(int N, int *data);



