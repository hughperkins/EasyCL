#include "EasyCL.h"

#include "gtest/gtest.h"

#include <iostream>
#include <stdexcept>
using namespace std;

TEST(testbuildlog, main ) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    try {
        cl->buildKernel( "testbuildlog.cl", "foo" );
    } catch( std::runtime_error err ) {
        cout << err.what() << endl;
        EXPECT_TRUE( string( err.what() ).find( "testbuildlog.cl" ) != string::npos );
    }
    delete cl;
}

