#pragma once

#include "EasyCL_export.h"

namespace easycl {

class EasyCL_EXPORT CLArray {
public:
    int N;
    bool onHost;
    bool onDevice;

    cl_mem devicearray;
    EasyCL *cl; // NOT owned by this object, so dont free!

    cl_int error;

    CLArray(int N, EasyCL *cl) {
        this->N = N;
        this->cl = cl;
        error = CL_SUCCESS;

        onDevice = false;
        onHost = false;
    }
    virtual ~CLArray() {
        if(onDevice) {
            clReleaseMemObject(devicearray);                    
        }
    }
    virtual void allocateHostArray(int N) = 0;
    virtual void deleteHostArray() = 0;
    virtual void *getHostArray() = 0;
    void createOnDevice() {
        assert(!onHost && !onDevice);
        devicearray = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE, getElementSize() * N, 0, &error);
        assert(error == CL_SUCCESS);        
        onDevice = true;        
    }
    void copyToDevice() {
        assert(onHost && !onDevice);
        devicearray = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, getElementSize() * N, getHostArray(), &error);
        cl->checkError(error);
        onDevice = true;
    }
    void moveToDevice() {
        assert(onHost && !onDevice);
        devicearray = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, getElementSize() * N, getHostArray(), &error);
        assert(error == CL_SUCCESS);
        deleteHostArray();
        onDevice = true;
        onHost = false;
    }
    void moveToHost() {
       copyToHost();
       deleteFromDevice();
    }
    void copyToHost() {
        assert(onDevice);
        if(!onHost) {
            allocateHostArray(N);
            onHost = true;                
        }
        error = clEnqueueReadBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, NULL);    
        cl->checkError(error);
    }
    void deleteFromHost(){
        assert(onHost);
        deleteHostArray();
        onHost = false;
    }
    cl_mem *getDeviceArray() {
        if(!onDevice) {
            assert(onHost);
            copyToDevice();
            deleteFromHost();
        }
        return &devicearray;
    }
    void deleteFromDevice(){
        assert(onDevice);
        clReleaseMemObject(devicearray);        
        onDevice = false;
    }
    inline int size() {
        return N;
    }
    inline bool isOnHost(){
        return onHost;
    }
    inline bool isOnDevice(){
        return onDevice;
    }
    virtual int getElementSize() = 0;
};
}
