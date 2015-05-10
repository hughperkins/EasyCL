// Copyright Hugh Perkins 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include "EasyCL.h"

#include "CLWrapper.h"

CLWrapper::CLWrapper( int N, EasyCL *cl ) : N(N), onHost(true), cl(cl) {
    error = CL_SUCCESS;
    onDevice = false;
    deviceDirty = false;
}
CLWrapper::CLWrapper( const CLWrapper &source ) :
     N(0), onHost(true)
        { // copy constructor
    throw std::runtime_error("can't assign these...");
}
CLWrapper &CLWrapper::operator=( const CLWrapper &two ) { // assignment operator
   if( this == &two ) { // self-assignment
      return *this;
   }
   throw std::runtime_error("can't assign these...");
}
CLWrapper::~CLWrapper() {
    if( onDevice ) {
//            std::cout << "releasing device array of " << N << " elements" << std::endl;
        clReleaseMemObject(devicearray);                    
    }
}
EasyCL *CLWrapper::getCl() {
    return cl;
}
void CLWrapper::deleteFromDevice(){
    if(!onDevice) {
        throw std::runtime_error("deletefromdevice(): not on device");
    }
//        std::cout << "deleted device array of " << N << " elements" << std::endl;
    clReleaseMemObject(devicearray);        
    onDevice = false;
    deviceDirty = false;
}
cl_mem *CLWrapper::getDeviceArray() {
    if( !onDevice ) {
        if(!onHost ) {
            throw std::runtime_error("getDeviceArray(): not on device, and not on host");
        }
//            std::cout << "copy array to device of " << N << " elements" << std::endl;
        copyToDevice();
    }
    return &devicearray;
}
void CLWrapper::createOnDevice() {
    if(onDevice) {
        throw std::runtime_error("createOnDevice(): already on device");
    }
//        std::cout << "creating buffer on device of " << N << " elements" << std::endl;
    devicearray = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE, getElementSize() * N, 0, &error);
    cl->checkError(error);
    onDevice = true;
    deviceDirty = false;
//        std::cout << "... created ok" << std::endl;
}
void CLWrapper::copyToHost() {
    if(!onDevice) {
        throw std::runtime_error("copyToHost(): not on device");
    }
    cl->finish();
    error = clEnqueueReadBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, NULL);    
    cl->checkError(error);
    deviceDirty = false;
}
cl_mem CLWrapper::getBuffer() { // be careful!
    return devicearray;
}
void CLWrapper::copyToDevice() {
    if(!onHost ) {
        throw std::runtime_error("copyToDevice(): not on host");
    }
    if( onDevice ) {
        error = clEnqueueWriteBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, NULL);    
        cl->checkError(error);
        deviceDirty = false;
    } else {
//        std::cout << "copying buffer to device of " << N << " elements" << std::endl;
//        for( int i = 0; i < N; i++ ) { 
//           std::cout << "i " << i << " " << ((float*)getHostArrayConst())[i] << std::endl;
//        }
        devicearray = clCreateBuffer(*(cl->context), CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, getElementSize() * N, (void *)getHostArrayConst(), &error);
        cl->checkError(error);
        onDevice = true;
        deviceDirty = false;
    }
}

int CLWrapper::size() {
    return N;
}
bool CLWrapper::isOnHost(){
    return onHost;
}
bool CLWrapper::isOnDevice(){
    return onDevice;
}
bool CLWrapper::isDeviceDirty() {
    return deviceDirty;
}
void CLWrapper::markDeviceDirty() {
    deviceDirty = true;
}

