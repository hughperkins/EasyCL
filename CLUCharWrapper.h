// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <stdexcept>

#include "EasyCL.h"
#include "CLWrapper.h"

namespace easycl {
// this wraps an existing array, which we wont free, rather than creating a new array
// probably more efficient....
class CLUCharWrapper : public CLWrapper {
protected:
    unsigned char *hostarray;  // NOT owned by this object, do NOT free :-)
public:
    CLUCharWrapper(int N, unsigned char *_hostarray, EasyCL *easycl) : 
             CLWrapper(N, easycl),
             hostarray(_hostarray) 
              {
        error = CL_SUCCESS;

        onDevice = false;
    }
    CLUCharWrapper(const CLUCharWrapper &source) :
        CLWrapper(0, 0), hostarray(0) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLUCharWrapper &operator=(const CLUCharWrapper &two) { // assignment operator
       if(this == &two) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    virtual ~CLUCharWrapper() {
    }
    virtual int getElementSize() {
        return sizeof(hostarray[0]);
    }
    virtual void *getHostArray() {
        return hostarray;
    }
    virtual void const*getHostArrayConst() {
        return hostarray;
    }
};
}
