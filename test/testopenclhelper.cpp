#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "OpenCLHelper.h"
#include "CLKernel.h"

#include "test/asserts.h"

TEST( testopenclhelper, main ) {
    cout << "start" << endl;
    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    OpenCLHelper cl;
    CLKernel *kernel = cl.buildKernel("testopenclhelper.cl", "test");
    float in[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    float out[5];
    kernel->input( 5, in );
    kernel->output( 5, out );
    size_t global = 5;
    size_t local = 5;
    kernel->run(1, &global, &local );
    assertEquals( out[0] , 7 );
    assertEquals( out[1] , 10 );
    assertEquals( out[2] , 13 );
    assertEquals( out[3] , 16 );
    assertEquals( out[4] , 19 );
    cout << "tests completed ok" << endl;
}

TEST( testopenclhelper, power2helper ) {
    ASSERT_EQ( 1, OpenCLHelper::getPower2Upperbound( 1 ) );
    ASSERT_EQ( 2, OpenCLHelper::getPower2Upperbound( 2 ) );
    ASSERT_EQ( 4, OpenCLHelper::getPower2Upperbound( 3 ) );
    ASSERT_EQ( 4, OpenCLHelper::getPower2Upperbound( 4 ) );
    ASSERT_EQ( 8, OpenCLHelper::getPower2Upperbound( 5 ) );
    ASSERT_EQ( 8, OpenCLHelper::getPower2Upperbound( 6 ) );
    ASSERT_EQ( 8, OpenCLHelper::getPower2Upperbound( 7 ) );
    ASSERT_EQ( 8, OpenCLHelper::getPower2Upperbound( 8 ) );
    ASSERT_EQ( 16, OpenCLHelper::getPower2Upperbound( 9 ) );
    ASSERT_EQ( 16, OpenCLHelper::getPower2Upperbound( 15 ) );
    ASSERT_EQ( 16, OpenCLHelper::getPower2Upperbound( 16 ) );
    ASSERT_EQ( 32, OpenCLHelper::getPower2Upperbound( 17 ) );

    ASSERT_EQ( 1, OpenCLHelper::getNextPower2( 1 ) );
    ASSERT_EQ( 2, OpenCLHelper::getNextPower2( 2 ) );
    ASSERT_EQ( 4, OpenCLHelper::getNextPower2( 3 ) );
    ASSERT_EQ( 4, OpenCLHelper::getNextPower2( 4 ) );
    ASSERT_EQ( 8, OpenCLHelper::getNextPower2( 5 ) );
    ASSERT_EQ( 8, OpenCLHelper::getNextPower2( 6 ) );
    ASSERT_EQ( 8, OpenCLHelper::getNextPower2( 7 ) );
    ASSERT_EQ( 8, OpenCLHelper::getNextPower2( 8 ) );
    ASSERT_EQ( 16, OpenCLHelper::getNextPower2( 9 ) );
    ASSERT_EQ( 16, OpenCLHelper::getNextPower2( 15 ) );
    ASSERT_EQ( 16, OpenCLHelper::getNextPower2( 16 ) );
    ASSERT_EQ( 32, OpenCLHelper::getNextPower2( 17 ) );
}


