#include <iostream>
#include <cstdlib>
using namespace std;

#include "OpenCLHelper.h"
#include "CLKernel.h"
#include "CLArray1d.h"

#include "test/asserts.h"

int main( int argc, char *argv[] ) {
    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    OpenCLHelper cl(0);
    CLKernel *kernel = cl.buildKernel("../test/testopenclhelper.cl", "test");
    CLArray1d *out = cl.array1d(5);
    float in[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    out->createOnDevice();
    kernel->input( 5, in );
    kernel->output( out );
    size_t global = 5;
    size_t local = 5;
    kernel->run(1, &global, &local );
    assertEquals( (*out)[0] , 7 );
    assertEquals( (*out)[1] , 10 );
    assertEquals( (*out)[2] , 13 );
    assertEquals( (*out)[3] , 16 );
    assertEquals( (*out)[4] , 19 );
    cout << "tests completed ok" << endl;
}

