// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <stdexcept>
//#include <CL/cl.h>

#include "clcc/clew.h"

#include "OpenCLHelper.h"

class CLArrayInt {
protected:
    int N;
    bool onHost;
    bool onDevice;

    int *hostarray;
    cl_mem devicearray;
    OpenCLHelper *openclhelper; // NOT owned by this object, so dont free!

    cl_int error;

public:
    CLArrayInt( int N, OpenCLHelper *openclhelper ) {
        this->N = N;
        this->openclhelper = openclhelper;
        error = CL_SUCCESS;

        onDevice = false;
        onHost = false;

        hostarray = 0;
    }
    CLArrayInt( const CLArrayInt &source ) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLArrayInt &operator=( const CLArrayInt &two ) { // assignment operator
       if( this == &two ) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    ~CLArrayInt() {
        if( onHost ) {
            delete[] hostarray;
//            cout << "deleted hostarray of " << N << " floats" << endl;
        }
        if( onDevice ) {
            clReleaseMemObject(devicearray);                    
//            cout << "deleted device array of " << N << " floats" << endl;
        }
    }
    void createOnHost() {
        assert(!onHost && !onDevice);
        hostarray = new int[N];
//        cout << "allocated hostarray of " << N << " floats" << endl;
        onHost = true;
    }
    void createOnDevice() {
        assert(!onHost && !onDevice);
        devicearray = clCreateBuffer(openclhelper->context, CL_MEM_READ_WRITE, sizeof(int) * N, 0, &error);
//        cout << "allocated device array of " << N << " floats" << endl;
        assert( error == CL_SUCCESS );        
        onDevice = true;        
    }
    void copyToHost() {
        assert( !onHost && onDevice );
        hostarray = new int[N];
        error = clEnqueueReadBuffer(openclhelper->queue, devicearray, CL_TRUE, 0, sizeof(int) * N, hostarray, 0, NULL, NULL);    
        openclhelper->checkError( error );
//        cout << "allocated host array of " << N << " floats" << endl;
        onHost = true;                
    }
    void copyToDevice() {
        assert( onHost && !onDevice );
        devicearray = clCreateBuffer(openclhelper->context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * N, (void *)hostarray, &error);
        openclhelper->checkError(error);
        onDevice = true;
//        cout << "allocated device array of " << N << " floats" << endl;
    }
    void moveToDevice() {
        assert( onHost && !onDevice );
        devicearray = clCreateBuffer(openclhelper->context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * N, (void *)hostarray, &error);
        assert(error == CL_SUCCESS);
        delete[] hostarray;
//        cout << "deleted hostarray of " << N << " floats" << endl;
//        cout << "allocated device array of " << N << " floats" << endl;
        onDevice = true;
        onHost = false;
    }
    cl_mem *getDeviceArray() {
        if( !onDevice ) {
            assert( onHost );
            copyToDevice();
            deleteFromHost();
        }
        return &devicearray;
    }
    void deleteFromHost(){
        assert(onHost);
//        cout << "deleted hostarray of " << N << " floats" << endl;
        delete[] hostarray;
        onHost = false;
    }
    void deleteFromDevice(){
        assert(onDevice);
//        cout << "deleted device array of " << N << " floats" << endl;
        clReleaseMemObject(devicearray);        
        onDevice = false;
    }
    int &operator[]( int n ) {
        if( !onHost ) {
            if( !onDevice ) {
                throw runtime_error("array not present either on host or device!");
            }
            copyToHost();
        }
        return hostarray[n];
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

