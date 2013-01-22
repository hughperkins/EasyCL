#include <iostream>
#include <cstdlib>
using namespace std;

#include "OpenCLHelper.h"
#include "CLKernel.h"

int main( int argc, char *argv[] ) {
    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    OpenCLHelper cl(0);
    CLKernel *kernel = cl.buildKernel("../test/testinout.cl", "test");
    float inout[5];
    for( int i = 0; i < 5; i++ ) {
        inout[i] = i * 3;
    }
    kernel->inout( 5, inout );
    assert( inout[0] == 7 );
    assert( inout[1] == 10 );
    assert( inout[2] == 13 );
    assert( inout[3] == 16 );
    assert( inout[4] == 19 );
    cout << "tests completed ok" << endl;
}

