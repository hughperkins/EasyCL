// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <stdexcept>
//#include <CL/cl.h>

//#include "clcc/clew.h"

#include "OpenCLHelper.h"
#include "CLArray.h"

class CLArrayFloat : public CLArray {
protected:
    float *hostarray;
public:
    CLArrayFloat( int N, OpenCLHelper *openclhelper ) :
            CLArray( N, openclhelper ) {
        hostarray = 0;
    }
    CLArrayFloat( const CLArrayFloat &source ) :
        CLArray( 0, 0 ) { // copy constructor
        throw std::runtime_error("can't assign these...");
    }
    CLArrayFloat &operator=( const CLArrayFloat &two ) { // assignment operator
       if( this == &two ) { // self-assignment
          return *this;
       }
       throw std::runtime_error("can't assign these...");
    }
    virtual ~CLArrayFloat() {
        if( onHost ) {
            deleteHostArray();
        }
    }
    virtual void createOnHost() {
        assert(!onHost && !onDevice);
        hostarray = new float[N];
        onHost = true;
    }
    virtual void allocateHostArray(int N) {
        hostarray = new float[N];
    }
    virtual void deleteHostArray() {
        delete[] hostarray;
    }
    virtual int getElementSize() {
        return sizeof( float );
    }
    virtual void *getHostArray() {
        return hostarray;
    }
    float &operator[]( int n ) {
        if( !onHost ) {
            if( !onDevice ) {
                throw std::runtime_error("array not present either on host or device!");
            }
            copyToHost();
        }
        return hostarray[n];
    }
};

