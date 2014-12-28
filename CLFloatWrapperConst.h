// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <stdexcept>

#include "OpenCLHelper.h"
#include "CLWrapper.h"

// this wraps an existing array, which we wont free, rather than creating a new array
// probably more efficient....
class CLFloatWrapperConst : public CLWrapper {
protected:
    float const*hostarray;  // NOT owned by this object, do NOT free :-)
public:
    CLFloatWrapperConst( int N, float const *_hostarray, OpenCLHelper *openclhelper ) : 
             hostarray(_hostarray),
             CLWrapper( N, openclhelper) {
    }
    CLFloatWrapperConst( const CLFloatWrapperConst &source ) :
        hostarray(0), CLWrapper( 0, 0 ) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLFloatWrapperConst &operator=( const CLFloatWrapperConst &two ) { // assignment operator
       if( this == &two ) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    virtual ~CLFloatWrapperConst() {
    }
    virtual int getElementSize() {
        return sizeof(hostarray[0]);
    }
    inline float get( int index ) {
        return hostarray[index];
    }
    virtual void *getHostArray() {
        throw std::runtime_error("gethostarray() not implemented for clfloatwrapperconst");
    }
    virtual void const *getHostArrayConst() {
        return hostarray;
    }
};

