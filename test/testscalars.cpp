#include <iostream>
#include <cstdlib>
using namespace std;

#include "OpenCLHelper.h"
#include "CLKernel.h"

#include "test/asserts.h"

int main( int argc, char *argv[] ) {
    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    OpenCLHelper cl;
    CLKernel *kernel = cl.buildKernel("../test/testscalars.cl", "test");
    int intout[5];
    float floatout[5];
    kernel->input( 37 );
    kernel->input( 1.234f);
    kernel->output( 5, intout );
    kernel->output( 5, floatout );
    size_t global = 5;
    size_t local = 5;
    kernel->run( 1, &global, &local );

    for( int i = 0; i < 5; i++ ) {
        cout << intout[i] << " ";
    }
    cout << endl;
    for( int i = 0; i < 5; i++ ) {
        cout << floatout[i] << " ";
    }
    cout << endl;

    assertEquals( intout[0], 37 );
    assertEquals( intout[1] , 38 );
    assertEquals( intout[2] , 39 );
    assertEquals( intout[3] , 40 );
    assertEquals( intout[4], 41 );
    assertEquals( floatout[0] , 1.234f );
    assertEquals( floatout[1] , 2.234f );
    assertEquals( floatout[2] , 3.234f );
    assertEquals( floatout[3] , 4.234f );
    assertEquals( floatout[4] , 5.234f );
    cout << "tests completed ok" << endl;
}

