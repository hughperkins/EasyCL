// Copyright Hugh Perkins 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

// #include "EasyCL.h"

#include "EasyCL_export.h"

namespace easycl {
//class cl_mem;
//class cl_int;
class EasyCL;

class EasyCL_EXPORT CLWrapper {
protected:
    const int N;
    const bool onHost;
    bool onDevice;

    bool deviceDirty; // have we updated the device copy, without copying to host?

    cl_mem devicearray;
    EasyCL *cl; // NOT owned by this object, so dont free!

    cl_int error;

public:
    virtual int getElementSize() = 0;
    virtual void *getHostArray() = 0;
    virtual void const *getHostArrayConst() = 0;
    virtual void copyTo(CLWrapper *target);
    virtual void copyTo(CLWrapper *target, int srcOffset, int dstOffset, int count);

    CLWrapper(int N, EasyCL *cl);
    CLWrapper(const CLWrapper &source);
    CLWrapper &operator=(const CLWrapper &two);
    virtual ~CLWrapper();
    virtual EasyCL *getCl();
    virtual void deleteFromDevice();
    virtual cl_mem *getDeviceArray();
    virtual void createOnDevice();
    virtual void copyToHost();
    virtual cl_mem getBuffer();
    virtual void copyToDevice();

    virtual int size();
    virtual bool isOnHost();
    virtual bool isOnDevice();
    virtual bool isDeviceDirty();
    virtual void markDeviceDirty();
};
}
