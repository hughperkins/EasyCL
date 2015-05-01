#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"
#include "CLArrayFloat.h"

#include "test/asserts.h"

TEST( testclarray, main ) {
    if( !EasyCL::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    CLArrayFloat *out = cl->arrayFloat(5);
    float in[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    kernel->in( 5, in )->out( out )->run_1d( 5, 5 );
    assertEquals( (*out)[0] , 7 );
    assertEquals( (*out)[1] , 10 );
    assertEquals( (*out)[2] , 13 );
    assertEquals( (*out)[3] , 16 );
    assertEquals( (*out)[4] , 19 );
    cout << "tests completed ok" << endl;

    delete out;
    delete kernel;
    delete cl;
}

