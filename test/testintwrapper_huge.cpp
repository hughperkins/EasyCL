#include <iostream>
#include <cstdlib>
// using namespace std;

#include "OpenCLHelper.h"

#include "test/asserts.h"

using namespace std;

int main( int argc, char *argv[] ) {
    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    OpenCLHelper cl;
    CLKernel *kernel = cl.buildKernel("../test/testopenclhelper.cl", "test_stress");
    const int N = 4500000;
    int *in = new int[N];
    for( int i = 0; i < N; i++ ) {
        in[i] = i * 3;
    }
    int *out = new int[N];
    CLWrapper *inwrapper = cl.wrap(N, in);
    CLWrapper *outwrapper = cl.wrap(N, out);
    inwrapper->copyToDevice();
    kernel->input( inwrapper );
    kernel->output( outwrapper );
    int globalSize = N;
    int workgroupsize = cl.getMaxWorkgroupSize();
    globalSize = ( ( globalSize + workgroupsize - 1 ) / workgroupsize ) * workgroupsize;
    cout << "globalsize: " << globalSize << " workgroupsize " << workgroupsize << endl;
    kernel->run_1d( globalSize, workgroupsize );
    outwrapper->copyToHost();
    for( int i = 0; i < N; i++ ) {
       if( out[i] != 500000 ) {
           cout << "out[" << i << "] != 500000: " << out[i] << endl;
           exit(-1);
       }
    }
//    assertEquals( out[0] , 500000 );
//    assertEquals( out[1] , 500000 );
//    assertEquals( out[2] , 500000 );
//    assertEquals( out[3] , 500000 );
//    assertEquals( out[4] , 500000 );
//    assertEquals( out[4000000] , 500000 );
    cout << "tests completed ok" << endl;
}

