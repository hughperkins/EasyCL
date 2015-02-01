#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "OpenCLHelper.h"

#include "test/asserts.h"

using namespace std;

TEST( testfloatwrapper, main ) {
    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testopenclhelper.cl", "test");
    float in[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    float out[5];
    CLWrapper *inwrapper = cl->wrap(5, in);
    CLWrapper *outwrapper = cl->wrap(5, out);
    inwrapper->copyToDevice();
    kernel->input( inwrapper );
    kernel->output( outwrapper );
    kernel->run_1d( 5, 5 );
    outwrapper->copyToHost();
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

TEST( testfloatwrapper, singlecopytodevice ) {
    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();
    float hostarray[1];
    hostarray[0] = 3.56f;
    CLWrapper *wrapper = cl->wrap(1, hostarray );
    wrapper->copyToDevice();
    hostarray[0] = 7.14f;
    wrapper->copyToHost();
    ASSERT_FLOAT_NEAR( 3.56f, hostarray[0] );

    delete wrapper;
    delete cl;
}

TEST( testfloatwrapper, doublecopytodevice ) {
    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();
    float hostarray[1];
    hostarray[0] = 3.56f;
    CLWrapper *wrapper = cl->wrap(1, hostarray );
    wrapper->copyToDevice();
    hostarray[0] = 4.12f;
    wrapper->copyToDevice();
    hostarray[0] = 7.14f;
    wrapper->copyToHost();
    ASSERT_FLOAT_NEAR( 4.12f, hostarray[0] );

    delete wrapper;
    delete cl;
}


