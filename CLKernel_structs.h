// only include this if you're passing structs
// otherwise no need
// downside of including this is, increase your compile time very slightly

#pragma once

#include "CLKernel.h"

namespace easycl {
template<typename T> CLKernel *CLKernel::input(int N, const T *data) {
	cl_mem buffer = clCreateBuffer(*(cl->context), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(T) * N, (void *)data, &error);
	cl->checkError(error);
	error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
	cl->checkError(error);
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
	cl_mem buffer = clCreateBuffer(*(cl->context), CL_MEM_WRITE_ONLY, sizeof(T) * N, 0, &error);
	cl->checkError(error);
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
	cl_mem buffer = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(T) * N, (void *)data, &error);
	cl->checkError(error);
	error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
	cl->checkError(error);
	buffers.push_back(buffer);
	outputArgBuffers.push_back(buffer);
	outputArgPointers.push_back((void *)(data) );
	outputArgSizes.push_back(sizeof(T) * N);
	nextArg++;
	return this;
}
}
