// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "OpenCLHelper.h"

class CLWrapper {
protected:
    const int N;
    const bool onHost;
    bool onDevice;

    cl_mem devicearray;
    OpenCLHelper *openclhelper; // NOT owned by this object, so dont free!

    cl_int error;

public:
    CLWrapper( int N, OpenCLHelper *openclhelper ) : N(N), openclhelper(openclhelper), onHost(true) {
        error = CL_SUCCESS;
        onDevice = false;
    }
    virtual ~CLWrapper() {
        if( onDevice ) {
            clReleaseMemObject(devicearray);                    
        }
    }
    virtual void deleteFromDevice(){
        assert(onDevice);
//        cout << "deleted device array of " << N << " floats" << endl;
        clReleaseMemObject(devicearray);        
        onDevice = false;
    }
    virtual cl_mem *getDeviceArray() {
        if( !onDevice ) {
            assert( onHost );
            copyToDevice();
        }
        return &devicearray;
    }
    virtual int getElementSize() = 0;
    virtual void *getHostArray() = 0;
    virtual void const *getHostArrayConst() = 0;
    virtual void createOnDevice() {
        assert( !onDevice);
        devicearray = clCreateBuffer(*(openclhelper->context), CL_MEM_READ_WRITE, getElementSize() * N, 0, &error);
        assert( error == CL_SUCCESS );        
        onDevice = true;        
    }
    virtual void copyToHost() {
        assert( onDevice );
        error = clEnqueueReadBuffer(*(openclhelper->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, NULL);    
        openclhelper->checkError( error );
    }
    virtual void copyToDevice() {
        assert( onHost && !onDevice );
        devicearray = clCreateBuffer(*(openclhelper->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, getElementSize() * N, (void *)getHostArrayConst(), &error);
        openclhelper->checkError(error);
        onDevice = true;
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
};

