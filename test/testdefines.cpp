#include <iostream>

#include "gtest/gtest.h"

#include "OpenCLHelper.h"

using namespace std;

TEST( testdefines, simple ) {
    OpenCLHelper cl;
    CLKernel *kernel = cl.buildKernel("../test/testdefines.cl", "testDefines", 
        "-D DOUBLE -D SOME_VALUE=5" );
    float out[32];
    kernel->out( 32, out );
    kernel->run_1d( 32, 32 );
    EXPECT_EQ( 10, out[3] );

    delete kernel;
}

