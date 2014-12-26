#pragma once

#include "CLArrayInt.h"
#include "CLArrayFloat.h"

class CLKernel {
    OpenCLHelper *openclhelper; // NOT owned by this object, dont delete!
    cl_kernel kernel;
    cl_int error;

    int nextArg;
public:
    CLKernel( OpenCLHelper *openclhelper, cl_kernel kernel ) {
        this->openclhelper = openclhelper;
        nextArg = 0;
        error = CL_SUCCESS;
        this->kernel = kernel;
    }
    CLKernel( const CLKernel &kernel ) {
        throw std::runtime_error("can't assign CLKernel");
    }
    CLKernel &operator=( const CLKernel &kernel ) {
        throw std::runtime_error("can't assign CLKernel");
    }
    ~CLKernel() {
        clReleaseKernel(kernel);
    }

    std::vector<cl_mem> buffers;

    std::vector<int> inputArgInts;
    std::vector<float> inputArgFloats;

    std::vector<cl_mem> outputArgBuffers;
    std::vector<void *> outputArgPointers;
    std::vector<size_t> outputArgSizes;

    CLKernel *input( CLArray *clarray1d );
    CLKernel *inout( CLArray *clarray1d );
    CLKernel *output( CLArray *clarray1d );

    CLKernel *in( CLArray *clarray1d ) { return input( clarray1d ); }
    CLKernel *out( CLArray *clarray1d ) { return output( clarray1d ); }

    CLKernel *input( CLWrapper *wrapper );
    CLKernel *output( CLWrapper *wrapper );

    CLKernel *in( CLWrapper *wrapper ) { return input( wrapper ); }
    CLKernel *out( CLWrapper *wrapper ) { return output( wrapper ); }

    template<typename T>
    CLKernel *input( int N, const T *data ) {
        cl_mem buffer = clCreateBuffer(*(openclhelper->context), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(T) * N, (void *)data, &error);
        assert(error == CL_SUCCESS);
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        openclhelper->checkError(error);
        buffers.push_back(buffer);
        nextArg++;
        return this;
    }
    template<typename T>
    CLKernel *in( int N, const T *data ) {
        return input( N, data );
    }
    CLKernel *input( int value ) {
        inputArgInts.push_back(value);
        error = clSetKernelArg(kernel, nextArg, sizeof(int), &(inputArgInts[inputArgInts.size()-1]));
        openclhelper->checkError(error);
        nextArg++;
        return this;
    }
    CLKernel *in( int value ) {
        return input( value );
    }
    CLKernel *input( float value ) {
        inputArgFloats.push_back(value);
        error = clSetKernelArg(kernel, nextArg, sizeof(float), &(inputArgFloats[inputArgFloats.size()-1]));
        openclhelper->checkError(error);
        nextArg++;
        return this;
    }
    CLKernel *in( float value ) {
        return input( value );
    }
    void local( int N ) {
        error = clSetKernelArg(kernel, nextArg, sizeof(float) * N, 0);
        openclhelper->checkError(error);
        nextArg++;
    }
    template<typename T>
    CLKernel *output( int N, T *data ) {
        cl_mem buffer = clCreateBuffer(*(openclhelper->context), CL_MEM_WRITE_ONLY, sizeof(T) * N, 0, &error);
        assert( error == CL_SUCCESS );
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        buffers.push_back(buffer);
        //outputArgNums.push_back(nextArg);
        outputArgBuffers.push_back(buffer);
        outputArgPointers.push_back(data);
        outputArgSizes.push_back(sizeof(T) * N );
        nextArg++;
        return this;
    }
    template<typename T>
    CLKernel *out( int N, T *data ) {
        return output( N, data );
    }
    template<typename T>
    CLKernel *inout( int N, T *data ) {
        cl_mem buffer = clCreateBuffer(*(openclhelper->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(T) * N, (void *)data, &error);
        assert(error == CL_SUCCESS);
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        openclhelper->checkError(error);
        buffers.push_back(buffer);
        outputArgBuffers.push_back(buffer);
        outputArgPointers.push_back(data);
        outputArgSizes.push_back(sizeof(T) * N );
        nextArg++;
        return this;
    }
    void run_1d( int global_worksize, int local_worksize ) {
        size_t global_ws = global_worksize;
        size_t local_ws = local_worksize;
        run( 1, &global_ws, &local_ws );
    }

    void run(int ND, const size_t *global_ws, const size_t *local_ws ) {
        //cout << "running kernel" << std::endl;
        error = clEnqueueNDRangeKernel(*(openclhelper->queue), kernel, ND, NULL, global_ws, local_ws, 0, NULL, NULL);
        switch( error ) {
            case 0:
                break;
            case -4:
                std::cout << "Memory object allocation failure, code -4" << std::endl;
                exit(-1);
                break;
            case -5:
                std::cout << "Out of resources, code -5" << std::endl;
                exit(-1);
                break;
            case -11:
                std::cout << "Program build failure, code -11" << std::endl;
                exit(-1);
                break;
            case -46:
                std::cout << "Invalid kernel name, code -46" << std::endl;
                exit(-1);
                break;
            case -52:
                std::cout << "Invalid kernel args, code -52" << std::endl;
                exit(-1);
                break;
            case -54:
                std::cout << "Invalid work group size, code -54" << std::endl;
                exit(-1);
                break;
            default:
                std::cout << "Something went wrong, code " << error << std::endl;
                exit(-1);
        }
        openclhelper->checkError(error);
//        error = clFinish(openclhelper->queue);
//        openclhelper->checkError(error);
    //}

    //void retrieveresultsandcleanup() {
        for( int i = 0; i < outputArgBuffers.size(); i++ ) {
            clEnqueueReadBuffer(*(openclhelper->queue), outputArgBuffers[i], CL_TRUE, 0, outputArgSizes[i], outputArgPointers[i], 0, NULL, NULL);            
        }
        std::cout << "done" << std::endl;
       
        for(int i = 0; i < buffers.size(); i++ ) {
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
};

