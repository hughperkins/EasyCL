// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "EasyCL.h"

#include "EasyCL_export.h"

class EasyCL_EXPORT CLWrapper {
protected:
    const int N;
    const bool onHost;
    bool onDevice;

    cl_mem devicearray;
    EasyCL *cl; // NOT owned by this object, so dont free!

    cl_int error;

public:
    CLWrapper( int N, EasyCL *cl ) : N(N), onHost(true), cl(cl) {
        error = CL_SUCCESS;
        onDevice = false;
    }
    CLWrapper( const CLWrapper &source ) :
         N(0), onHost(true)
            { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLWrapper &operator=( const CLWrapper &two ) { // assignment operator
       if( this == &two ) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    virtual ~CLWrapper() {
        if( onDevice ) {
//            std::cout << "releasing device array of " << N << " elements" << std::endl;
            clReleaseMemObject(devicearray);                    
        }
    }
    virtual EasyCL *getCl() {
        return cl;
    }
    virtual void deleteFromDevice(){
        if(!onDevice) {
            throw std::runtime_error("deletefromdevice(): not on device");
        }
//        std::cout << "deleted device array of " << N << " elements" << std::endl;
        clReleaseMemObject(devicearray);        
        onDevice = false;
    }
    virtual cl_mem *getDeviceArray() {
        if( !onDevice ) {
            if(!onHost ) {
                throw std::runtime_error("getDeviceArray(): not on device, and not on host");
            }
//            std::cout << "copy array to device of " << N << " elements" << std::endl;
            copyToDevice();
        }
        return &devicearray;
    }
    virtual int getElementSize() = 0;
    virtual void *getHostArray() = 0;
    virtual void const *getHostArrayConst() = 0;
    virtual void createOnDevice() {
        if(onDevice) {
            throw std::runtime_error("createOnDevice(): already on device");
        }
//        std::cout << "creating buffer on device of " << N << " elements" << std::endl;
        devicearray = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE, getElementSize() * N, 0, &error);
        cl->checkError(error);
        onDevice = true;        
//        std::cout << "... created ok" << std::endl;
    }
    virtual void copyToHost() {
        if(!onDevice) {
            throw std::runtime_error("copyToHost(): not on device");
        }
        cl->finish();
        error = clEnqueueReadBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, NULL);    
        cl->checkError(error);
    }
    virtual cl_mem getBuffer() { // be careful!
        return devicearray;
    }
    virtual void copyToDevice() {
        if(!onHost ) {
            throw std::runtime_error("copyToDevice(): not on host");
        }
        if( onDevice ) {
            error = clEnqueueWriteBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, NULL);    
            cl->checkError(error);               
        } else {
//        std::cout << "copying buffer to device of " << N << " elements" << std::endl;
//        for( int i = 0; i < N; i++ ) { 
//           std::cout << "i " << i << " " << ((float*)getHostArrayConst())[i] << std::endl;
//        }
            devicearray = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, getElementSize() * N, (void *)getHostArrayConst(), &error);
            cl->checkError(error);
            onDevice = true;
        }
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

