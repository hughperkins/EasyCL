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

#include <CL/cl.h>

// This code is partly based on http://opencl.codeplex.com/wikipage?title=OpenCL%20Tutorials%20-%201 , with helpers added for input and output arguments
class OpenCLHelper {
public:
     cl_int error;  

    cl_platform_id platform_id;
    cl_context context;
    cl_command_queue queue;
    cl_device_id device;
    cl_kernel kernel;

    int nextArg;

    static int roundUp( int quantization, int minimum ) {
        int size = ( minimum / quantization) * quantization;
        if( size < minimum ) {
            size += quantization;
        }
        return size;
    }

    OpenCLHelper(int gpuindex, string kernelfilepath, string kernelname ) {
        nextArg = 0;
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

        size_t src_size = 0;
        std::string path = kernelfilepath.c_str();
        std::string source = getFileContents(path);
        //cout << "source: " << source << endl;
        const char *source_char = source.c_str();
        src_size = source.length();
        cl_program program = clCreateProgramWithSource(context, 1, &source_char, &src_size, &error);
        assert(error == CL_SUCCESS);

        // Builds the program
        error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
        checkError(error);

        // Shows the log
        char* build_log;
        size_t log_size;
        // First call to know the proper size
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        build_log = new char[log_size+1];
        // Second call to get the log
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
        build_log[log_size] = '\0';
        if( log_size > 2 ) {
            cout << "build log: " << build_log << endl;
        }
        delete[] build_log;

        // Extracting the kernel
        kernel = clCreateKernel(program, kernelname.c_str(), &error);
        assert(error == CL_SUCCESS);
    }

    std::vector<cl_mem> buffers;
    std::vector<int> inputArgInts;
    std::vector<cl_mem> outputArgBuffers;
    std::vector<void *> outputArgPointers;
    std::vector<size_t> outputArgSizes;

    void input( int N, const float *data ) {
        cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, (void *)data, &error);
        assert(error == CL_SUCCESS);
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        checkError(error);
        buffers.push_back(buffer);
        nextArg++;
    }
    void input( int value ) {
        inputArgInts.push_back(value);
        error = clSetKernelArg(kernel, nextArg, sizeof(int), &(inputArgInts[inputArgInts.size()-1]));
        checkError(error);
        nextArg++;
    }
    void local( int N ) {
        error = clSetKernelArg(kernel, nextArg, sizeof(float) * N, 0);
        checkError(error);
        nextArg++;
    }
    void output( int N, float *data ) {
        cl_mem buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N, 0, &error);
        assert( error == CL_SUCCESS );
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        buffers.push_back(buffer);
        //outputArgNums.push_back(nextArg);
        outputArgBuffers.push_back(buffer);
        outputArgPointers.push_back(data);
        outputArgSizes.push_back(sizeof(float) * N );
        nextArg++;
    }

    void run(int ND, const size_t *global_ws, const size_t *local_ws ) {
        error = clEnqueueNDRangeKernel(queue, kernel, ND, NULL, global_ws, local_ws, 0, NULL, NULL);
        checkError(error);

        for( int i = 0; i < outputArgBuffers.size(); i++ ) {
            clEnqueueReadBuffer(queue, outputArgBuffers[i], CL_TRUE, 0, outputArgSizes[i], outputArgPointers[i], 0, NULL, NULL);            
        }

        clReleaseKernel(kernel);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);

        for(int i = 0; i < buffers.size(); i++ ) {
            clReleaseMemObject(buffers[i]);
        }
        buffers.clear();
        outputArgBuffers.clear();
        outputArgPointers.clear();
        outputArgSizes.clear();
        inputArgInts.clear();
    }
private:

string errorMessage(cl_int error ) {
    return toString(error);
}

void checkError( cl_int error ) {
    if( error != CL_SUCCESS ) {
        cout << "error: " << error << endl;
        assert (false);
    }
}

string getFileContents( string filename ) {
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
      if (buffer)
      {
        int result = fread (buffer, 1, length, f);
      }
      fclose (f);
      returnstring = buffer;
      delete[] buffer;
    }
    return returnstring;
}

template<typename T>
std::string toString(T val ) {
   std::ostringstream myostringstream;
   myostringstream << val;
   return myostringstream.str();
}

};

