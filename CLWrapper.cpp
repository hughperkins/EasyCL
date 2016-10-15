// Copyright Hugh Perkins 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include "EasyCL.h"

#include "CLWrapper.h"
#include "util/easycl_stringhelper.h"

namespace easycl {

CLWrapper::CLWrapper(int N, EasyCL *cl) : N(N), onHost(true), cl(cl) {
    error = CL_SUCCESS;
    onDevice = false;
    deviceDirty = false;
}
CLWrapper::CLWrapper(const CLWrapper &source) :
     N(0), onHost(true)
        { // copy constructor
    throw std::runtime_error("can't assign these...");
}
CLWrapper &CLWrapper::operator=(const CLWrapper &two) { // assignment operator
   if(this == &two) { // self-assignment
      return *this;
   }
   throw std::runtime_error("can't assign these...");
}
CLWrapper::~CLWrapper() {
    if(onDevice) {
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
    if(!onDevice) {
        if(!onHost) {
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
//    cl->finish();
    cl_event event = NULL;
    error = clEnqueueReadBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArray(), 0, NULL, &event);    
    cl->checkError(error);
    cl_int err = clWaitForEvents(1, &event);
    clReleaseEvent(event);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("wait for event on copytohost failed with " + easycl::toString(err) );
    }
    deviceDirty = false;
}
cl_mem CLWrapper::getBuffer() { // be careful!
    return devicearray;
}
void CLWrapper::copyToDevice() {
    if(!onHost) {
        throw std::runtime_error("copyToDevice(): not on host");
    }
    if(!onDevice) {
        createOnDevice();
    }
    error = clEnqueueWriteBuffer(*(cl->queue), devicearray, CL_TRUE, 0, getElementSize() * N, getHostArrayConst(), 0, NULL, NULL);    
    cl->checkError(error);
    deviceDirty = false;
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
void CLWrapper::copyTo(CLWrapper *target) {
    if(size() != target->size()) {
        throw std::runtime_error("copyTo: array size mismatch between source and target CLWrapper objects " + easycl::toString(size()) + " vs " + easycl::toString(target->size()));
    }
  copyTo(target, 0, 0, N);
}
void CLWrapper::copyTo(CLWrapper *target, int srcOffset, int dstOffset, int count) {
    if(!onDevice) {
        throw std::runtime_error("Must have called copyToDevice() or createOnDevice() before calling copyTo(CLWrapper*)");
    }
    if(!target->onDevice) {
        throw std::runtime_error("Must have called copyToDevice() or createOnDevice() on target before calling copyTo(target)");
    }
    if(srcOffset + count > N) {
      throw std::runtime_error("copyTo: not enough source elements, given offset " + easycl::toString(srcOffset) + " and count " + easycl::toString(count));
    }
    if(dstOffset + count > target->N) {
      throw std::runtime_error("copyTo: not enough destation elements, given offset " + easycl::toString(dstOffset) + " and count " + easycl::toString(count));
    }
    if(getElementSize() != target->getElementSize()) {
        throw std::runtime_error("copyTo: element size mismatch between source and target CLWrapper objects");
    }
    // can assume that we have our data on the device now, because of if check
    // just now
    // we will also assume that destination CLWrapper* is valid
//    cl_event event = NULL;
    cl_int err = clEnqueueCopyBuffer(*(cl->queue), devicearray, target->devicearray, 
        srcOffset * getElementSize(), dstOffset * getElementSize(), count * getElementSize(),
        0, NULL, 0);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("copyTo failed with " + easycl::toString(err) );
    }
    else {
        /* Wait for calculations to be finished. */
//        err = clWaitForEvents(1, &event);
    }
//    clReleaseEvent(event);
    target->markDeviceDirty();
}
}
