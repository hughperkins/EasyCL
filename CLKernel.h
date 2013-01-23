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
        throw runtime_error("can't assign CLKernel");
    }
    CLKernel &operator=( const CLKernel &kernel ) {
        throw runtime_error("can't assign CLKernel");
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

    std::vector<CLArrayFloat *> clarray1ds;

    void input( CLArrayFloat *clarray1d );
    void input( CLArrayInt *clarray1d );
    void output( CLArrayFloat *clarray1d );
    void output( CLArrayInt *clarray1d );
    void inout( CLArrayFloat *clarray1d );
    void inout( CLArrayInt *clarray1d );

    void input( int N, const float *data ) {
        cl_mem buffer = clCreateBuffer(openclhelper->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, (void *)data, &error);
        assert(error == CL_SUCCESS);
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        openclhelper->checkError(error);
        buffers.push_back(buffer);
        nextArg++;
    }
    void input( int N, const cl_int *data ) {
        cl_mem buffer = clCreateBuffer(openclhelper->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * N, (void *)data, &error);
        assert(error == CL_SUCCESS);
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        openclhelper->checkError(error);
        buffers.push_back(buffer);
        nextArg++;
    }
    void input( int value ) {
        inputArgInts.push_back(value);
        error = clSetKernelArg(kernel, nextArg, sizeof(int), &(inputArgInts[inputArgInts.size()-1]));
        openclhelper->checkError(error);
        nextArg++;
    }
    void input( float value ) {
        inputArgFloats.push_back(value);
        error = clSetKernelArg(kernel, nextArg, sizeof(int), &(inputArgFloats[inputArgFloats.size()-1]));
        openclhelper->checkError(error);
        nextArg++;
    }
    void local( int N ) {
        error = clSetKernelArg(kernel, nextArg, sizeof(float) * N, 0);
        openclhelper->checkError(error);
        nextArg++;
    }
    void output( int N, float *data ) {
        cl_mem buffer = clCreateBuffer(openclhelper->context, CL_MEM_WRITE_ONLY, sizeof(float) * N, 0, &error);
        assert( error == CL_SUCCESS );
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        buffers.push_back(buffer);
        //outputArgNums.push_back(nextArg);
        outputArgBuffers.push_back(buffer);
        outputArgPointers.push_back(data);
        outputArgSizes.push_back(sizeof(float) * N );
        nextArg++;
    }
    void output( int N, cl_int *data ) {
        cl_mem buffer = clCreateBuffer(openclhelper->context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * N, 0, &error);
        assert( error == CL_SUCCESS );
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        buffers.push_back(buffer);
        //outputArgNums.push_back(nextArg);
        outputArgBuffers.push_back(buffer);
        outputArgPointers.push_back(data);
        outputArgSizes.push_back(sizeof(cl_int) * N );
        nextArg++;
    }
    void inout( int N, float *data ) {
        cl_mem buffer = clCreateBuffer(openclhelper->context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, (void *)data, &error);
        assert(error == CL_SUCCESS);
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        openclhelper->checkError(error);
        buffers.push_back(buffer);
        outputArgBuffers.push_back(buffer);
        outputArgPointers.push_back(data);
        outputArgSizes.push_back(sizeof(float) * N );
        nextArg++;
    }
    void inout( int N, int *data ) {
        cl_mem buffer = clCreateBuffer(openclhelper->context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * N, (void *)data, &error);
        assert(error == CL_SUCCESS);
        error = clSetKernelArg(kernel, nextArg, sizeof(cl_mem), &buffer);
        openclhelper->checkError(error);
        buffers.push_back(buffer);
        outputArgBuffers.push_back(buffer);
        outputArgPointers.push_back(data);
        outputArgSizes.push_back(sizeof(int) * N );
        nextArg++;
    }

    void run(int ND, const size_t *global_ws, const size_t *local_ws ) {
        //cout << "running kernel" << endl;
        error = clEnqueueNDRangeKernel(openclhelper->queue, kernel, ND, NULL, global_ws, local_ws, 0, NULL, NULL);
        switch( error ) {
            case 0:
                break;
            case -4:
                cout << "Memory object allocation failure, code -4" << endl;
                exit(-1);
                break;
            case -5:
                cout << "Out of resources, code -5" << endl;
                exit(-1);
                break;
            case -11:
                cout << "Program build failure, code -11" << endl;
                exit(-1);
                break;
            case -46:
                cout << "Invalid kernel name, code -46" << endl;
                exit(-1);
                break;
            case -52:
                cout << "Invalid kernel args, code -52" << endl;
                exit(-1);
                break;
            case -54:
                cout << "Invalid work group size, code -54" << endl;
                exit(-1);
                break;
            default:
                cout << "Something went wrong, code " << error << endl;
                exit(-1);
        }
        openclhelper->checkError(error);
//        error = clFinish(openclhelper->queue);
//        openclhelper->checkError(error);
    //}

    //void retrieveresultsandcleanup() {
        for( int i = 0; i < outputArgBuffers.size(); i++ ) {
            clEnqueueReadBuffer(openclhelper->queue, outputArgBuffers[i], CL_TRUE, 0, outputArgSizes[i], outputArgPointers[i], 0, NULL, NULL);            
        }


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

