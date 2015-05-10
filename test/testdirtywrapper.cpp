// Copyright Hugh Perkins 2013, 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"

#include "test/asserts.h"

using namespace std;

TEST( testdirtywrapper, main ) {
    if( !EasyCL::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    float in[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    float out[5];
    CLWrapper *inwrapper = cl->wrap(5, in);
    CLWrapper *outwrapper = cl->wrap(5, out);
    EXPECT_FALSE( inwrapper->isDeviceDirty() );
    EXPECT_FALSE( outwrapper->isDeviceDirty() );

    inwrapper->copyToDevice();
    EXPECT_FALSE( inwrapper->isDeviceDirty() );
    EXPECT_FALSE( outwrapper->isDeviceDirty() );

    kernel->input( inwrapper );
    kernel->output( outwrapper );
    EXPECT_FALSE( inwrapper->isDeviceDirty() );
    EXPECT_FALSE( outwrapper->isDeviceDirty() );

    kernel->run_1d( 5, 5 );
    EXPECT_FALSE( inwrapper->isDeviceDirty() );
    EXPECT_TRUE( outwrapper->isDeviceDirty() );

    outwrapper->copyToHost();
    EXPECT_FALSE( outwrapper->isDeviceDirty() );

    assertEquals( out[0] , 7 );
    assertEquals( out[1] , 10 );
    assertEquals( out[2] , 13 );
    assertEquals( out[3] , 16 );
    assertEquals( out[4] , 19 );
    cout << "tests completed ok" << endl;

    delete inwrapper;
    delete outwrapper;
    delete kernel;
    delete cl;
}


