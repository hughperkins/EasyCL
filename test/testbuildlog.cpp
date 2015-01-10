#include "OpenCLHelper.h"

#include "gtest/gtest.h"

#include <iostream>
#include <stdexcept>
using namespace std;

TEST(testbuildlog, main ) {
    OpenCLHelper cl;
    try {
        cl.buildKernel( "../test/testbuildlog.cl", "foo" );
    } catch( std::runtime_error err ) {
        cout << err.what() << endl;
        EXPECT_TRUE( string( err.what() ).find( "testbuildlog.cl" ) != string::npos );
    }
}

