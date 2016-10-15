// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <stdexcept>
//#include <CL/cl.h>

//#include "clcc/clew.h"

#include "EasyCL.h"
#include "CLArray.h"

namespace easycl {

class CLArrayFloat : public CLArray {
protected:
    float *hostarray;
public:
    CLArrayFloat(int N, EasyCL *easycl) :
            CLArray(N, easycl) {
        hostarray = 0;
    }
    CLArrayFloat(const CLArrayFloat &source) :
        CLArray(0, 0) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLArrayFloat &operator=(const CLArrayFloat &two) { // assignment operator
       if(this == &two) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    virtual ~CLArrayFloat() {
        if(onHost) {
            deleteHostArray();
        }
    }
    virtual void createOnHost() {
        assert(!onHost && !onDevice);
        allocateHostArray(N);
    }
    virtual void allocateHostArray(int N) {
        hostarray = new float[N];
        onHost = true;
    }
    virtual void deleteHostArray() {
        delete[] hostarray;
        onHost = false;
    }
    virtual int getElementSize() {
        return sizeof(float);
    }
    virtual void *getHostArray() {
        return hostarray;
    }
//    float const &operator[](int n) {
//        if(!onHost) {
//            if(!onDevice) {
//                createOnHost();
//            } else {
//                copyToHost();
//            }
//        }
//        return hostarray[n];
//    }
    float &operator[](int n) {
        if(!onHost) {
            if(!onDevice) {
                createOnHost();
            } else {
                moveToHost();
            }
        }
        return hostarray[n];
    }
};
}
