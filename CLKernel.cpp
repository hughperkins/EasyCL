// Copyright Hugh Perkins 2013 hughperkins at gmail
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

#include "EasyCL_export.h"

template<typename T>
std::string CLKernel::toString(T val) {
	std::ostringstream myostringstream;
	myostringstream << val;
	return myostringstream.str();
}

CLKernel::CLKernel(EasyCL *easycl, cl_program program, cl_kernel kernel) {
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

CLKernel *CLKernel::input( CLArray *clarray1d ) {
    assert( clarray1d != 0 );
    if( !clarray1d->isOnDevice() ) {
        clarray1d->moveToDevice();
    }
    if( clarray1d->isOnHost() ) {
        clarray1d->deleteFromHost();
    }
    cl_mem *devicearray = clarray1d->getDeviceArray();
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), devicearray );
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::output( CLArray *clarray1d ) {
    assert( clarray1d != 0 );
    if( clarray1d->isOnHost() ) {
        clarray1d->deleteFromHost();
    }
    if( !clarray1d->isOnDevice() ) {
        clarray1d->createOnDevice();
    }
    assert( clarray1d->isOnDevice() && !clarray1d->isOnHost() );
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), (clarray1d->getDeviceArray()) );
    easycl->checkError(error);
    nextArg++;  
    return this;      
}

CLKernel *CLKernel::inout( CLArray *clarray1d ) {
    assert( clarray1d != 0 );
    if( !clarray1d->isOnDevice() ) {
        clarray1d->moveToDevice();
    }
    if( clarray1d->isOnHost() ) {
        clarray1d->deleteFromHost();
    }
    cl_mem *devicearray = clarray1d->getDeviceArray();
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), devicearray );
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::input( CLWrapper *wrapper ) {
    assert( wrapper != 0 );
    if( !wrapper->isOnDevice() ) {
        throw std::runtime_error("need to copyToDevice() before calling kernel->input");
    }
    cl_mem *devicearray = wrapper->getDeviceArray();
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), devicearray );
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::inout( CLWrapper *wrapper ) {
    assert( wrapper != 0 );
    if( !wrapper->isOnDevice() ) {
        throw std::runtime_error("need to copyToDevice() before calling kernel->input");
    }
    cl_mem *devicearray = wrapper->getDeviceArray();
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), devicearray );
    easycl->checkError(error);
    nextArg++;
    return this;
}

CLKernel *CLKernel::output( CLWrapper *wrapper ) {
    assert( wrapper != 0 );
    if( !wrapper->isOnDevice() ) {
        wrapper->createOnDevice();
    }
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), (wrapper->getDeviceArray()) );
    easycl->checkError(error);
    nextArg++;
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
CLKernel *CLKernel::input(int value) {
	inputArgInts.push_back(value);
	error = clSetKernelArg(kernel, nextArg, sizeof(int), &(inputArgInts[inputArgInts.size() - 1]));
	easycl->checkError(error);
	nextArg++;
	return this;
}
CLKernel *CLKernel::in(int value) {
	return input(value);
}
CLKernel *CLKernel::input(float value) {
	inputArgFloats.push_back(value);
	error = clSetKernelArg(kernel, nextArg, sizeof(float), &(inputArgFloats[inputArgFloats.size() - 1]));
	easycl->checkError(error);
	nextArg++;
	return this;
}
CLKernel *CLKernel::in(float value) {
	return input(value);
}
template<typename T>
CLKernel *CLKernel::output(int N, T *data) {
	cl_mem buffer = clCreateBuffer(*(easycl->context), CL_MEM_WRITE_ONLY, sizeof(T) * N, 0, &error);
	easycl->checkError(error);
	error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
	buffers.push_back(buffer);
	//outputArgNums.push_back(nextArg);
	outputArgBuffers.push_back(buffer);
	outputArgPointers.push_back( (void *)data);
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
	outputArgPointers.push_back( (void *)( data ) );
	outputArgSizes.push_back(sizeof(T) * N);
	nextArg++;
	return this;
}
void CLKernel::run_1d(int global_worksize, int local_worksize) {
	size_t global_ws = global_worksize;
	size_t local_ws = local_worksize;
	run(1, &global_ws, &local_ws);
}

void CLKernel::run(int ND, const size_t *global_ws, const size_t *local_ws) {
	//cout << "running kernel" << std::endl;
	error = clEnqueueNDRangeKernel(*(easycl->queue), kernel, ND, NULL, global_ws, local_ws, 0, NULL, NULL);
	switch (error) {
	case 0:
		break;
	case -4:
		throw std::runtime_error("Memory object allocation failure, code -4");
		break;
	case -5:
		throw std::runtime_error("Out of resources, code -5");
		break;
	case -11:
		throw std::runtime_error("Program build failure, code -11");
		break;
	case -46:
		throw std::runtime_error("Invalid kernel name, code -46");
		break;
	case -52:
		throw std::runtime_error("Invalid kernel args, code -52");
		break;
	case -54:
		throw std::runtime_error("Invalid work group size, code -54");
		break;
	default:
		throw std::runtime_error("Something went wrong, code " + toString(error));
	}
	easycl->checkError(error);
	//        error = clFinish(easycl->queue);
	//        easycl->checkError(error);
	//}

	//void retrieveresultsandcleanup() {
	for (int i = 0; i < (int)outputArgBuffers.size(); i++) {
		clEnqueueReadBuffer(*(easycl->queue), outputArgBuffers[i], CL_TRUE, 0, outputArgSizes[i], outputArgPointers[i], 0, NULL, NULL);
	}
	//        std::cout << "done" << std::endl;

	for (int i = 0; i < (int)buffers.size(); i++) {
		clReleaseMemObject(buffers[i]);
	}
	buffers.clear();
	outputArgBuffers.clear();
	outputArgPointers.clear();
	outputArgSizes.clear();
	inputArgInts.clear();
	inputArgFloats.clear();
	nextArg = 0;
}

// template class std::vector<cl_mem>;


template EasyCL_EXPORT CLKernel *CLKernel::input(int N, const float *data);
template EasyCL_EXPORT CLKernel *CLKernel::input(int N, const int *data);
template EasyCL_EXPORT CLKernel *CLKernel::in(int N, const float *data);
template EasyCL_EXPORT CLKernel *CLKernel::in(int N, const int *data);
template EasyCL_EXPORT CLKernel *CLKernel::output(int N, float *data);
template EasyCL_EXPORT CLKernel *CLKernel::output(int N, int *data);
template EasyCL_EXPORT CLKernel *CLKernel::out(int N, float *data);
template EasyCL_EXPORT CLKernel *CLKernel::out(int N, int *data);
template EasyCL_EXPORT CLKernel *CLKernel::inout(int N, float *data);
template EasyCL_EXPORT CLKernel *CLKernel::inout(int N, int *data);

