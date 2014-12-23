// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <stdexcept>

#include "OpenCLHelper.h"

// this wraps an existing array, which we wont free, rather than creating a new array
// probably more efficient....
class CLFloatWrapper {
protected:
    const int N;
    const bool onHost;
    bool onDevice;

    float *hostarray;  // NOT owned by this object, do NOT free :-)
    cl_mem devicearray;
    OpenCLHelper *openclhelper; // NOT owned by this object, so dont free!

    cl_int error;

public:
    CLFloatWrapper( int N, float *_hostarray, OpenCLHelper *openclhelper ) : 
             onHost(true),
             hostarray(_hostarray),
             N(N),
             openclhelper(openclhelper) {
        error = CL_SUCCESS;

        onDevice = false;
    }
    CLFloatWrapper( const CLFloatWrapper &source ) :
        onHost(false), hostarray(0), N(0), openclhelper(0) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLFloatWrapper &operator=( const CLFloatWrapper &two ) { // assignment operator
       if( this == &two ) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    virtual ~CLFloatWrapper() {
        if( onDevice ) {
            clReleaseMemObject(devicearray);                    
        }
    }
    virtual void createOnDevice() {
        assert( !onDevice);
        devicearray = clCreateBuffer(openclhelper->context, CL_MEM_READ_WRITE, sizeof(int) * N, 0, &error);
//        cout << "allocated device array of " << N << " floats" << endl;
        assert( error == CL_SUCCESS );        
        onDevice = true;        
    }
    virtual void copyToHost() {
        assert( onDevice );
        // hostarray = new int[N];
        error = clEnqueueReadBuffer(openclhelper->queue, devicearray, CL_TRUE, 0, sizeof(int) * N, hostarray, 0, NULL, NULL);    
        openclhelper->checkError( error );
//        cout << "allocated host array of " << N << " floats" << endl;
//        onHost = true;                
    }
    virtual void copyToDevice() {
        assert( onHost && !onDevice );
        devicearray = clCreateBuffer(openclhelper->context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * N, (void *)hostarray, &error);
        openclhelper->checkError(error);
        onDevice = true;
//        cout << "allocated device array of " << N << " floats" << endl;
    }
    virtual cl_mem *getDeviceArray() {
        if( !onDevice ) {
            assert( onHost );
            copyToDevice();
        }
        return &devicearray;
    }
    virtual void deleteFromDevice(){
        assert(onDevice);
//        cout << "deleted device array of " << N << " floats" << endl;
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
};

