// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include "CLKernel.h"
#include "CLArray1d.h"

void CLKernel::input( CLArray1d *clarray1d ) {
    assert( clarray1d != 0 );
    if( !clarray1d->isOnDevice() ) {
        clarray1d->moveToDevice();
    }
    if( clarray1d->isOnHost() ) {
        clarray1d->deleteFromHost();
    }
    cl_mem *devicearray = clarray1d->getDeviceArray();
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), devicearray );
    openclhelper->checkError(error);
    nextArg++;
}

void CLKernel::output( CLArray1d *clarray1d ) {
    assert( clarray1d != 0 );
    assert( clarray1d->isOnDevice() && !clarray1d->isOnHost() );
    error = clSetKernelArg( kernel, nextArg, sizeof(cl_mem), (clarray1d->getDeviceArray()) );
    openclhelper->checkError(error);
    nextArg++;        
}


