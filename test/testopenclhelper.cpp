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

    OpenCLHelper cl(0);
    CLKernel *kernel = cl.buildKernel("../test/testopenclhelper.cl", "test");
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

