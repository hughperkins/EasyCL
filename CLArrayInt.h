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

class CLArrayInt : public CLArray {
protected:
    int *hostarray;


public:
    CLArrayInt(int N, EasyCL *easycl) :
        CLArray(N, easycl) {
        hostarray = 0;
    }
    CLArrayInt(const CLArrayInt &source) :
        CLArray(0, 0) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLArrayInt &operator=(const CLArrayInt &two) { // assignment operator
       if(this == &two) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    virtual ~CLArrayInt() {
        if(onHost) {
            deleteHostArray();
        }
    }
    virtual void createOnHost() {
        assert(!onHost && !onDevice);
        hostarray = new int[N];
        onHost = true;
    }
    virtual int getElementSize() {
        return sizeof(int);
    }
    virtual void allocateHostArray(int N) {
        hostarray = new int[N];
    }
    virtual void deleteHostArray() {
        delete[] hostarray;
    }
    virtual void *getHostArray() {
        return hostarray;
    }
    int &operator[](int n) {
        if(!onHost) {
            if(!onDevice) {
                throw std::runtime_error("array not present either on host or device!");
            }
            copyToHost();
        }
        return hostarray[n];
    }
};
}
