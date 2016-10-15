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
class CLFloatWrapper : public CLWrapper {
protected:
    float *hostarray;  // NOT owned by this object, do NOT free :-)
public:
    CLFloatWrapper(int N, float *_hostarray, EasyCL *easycl) : 
             CLWrapper(N, easycl),
             hostarray(_hostarray)
              {
    }
    CLFloatWrapper(const CLFloatWrapper &source) :
        CLWrapper(0, 0), hostarray(0) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLFloatWrapper &operator=(const CLFloatWrapper &two) { // assignment operator
       if(this == &two) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    inline float get(int index) {
        return hostarray[index];
    }
    virtual ~CLFloatWrapper() {
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
