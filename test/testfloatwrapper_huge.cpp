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
    CLKernel *kernel = cl.buildKernel("../test/testopenclhelper.cl", "test");
    const int N = 30000;
    float in[N];
    for( int i = 0; i < N; i++ ) {
        in[i] = i * 3;
    }
    float out[N];
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
    assertEquals( out[0] , 7 );
    assertEquals( out[1] , 10 );
    assertEquals( out[2] , 13 );
    assertEquals( out[3] , 16 );
    assertEquals( out[4] , 19 );
    assertEquals( out[20000] , 20000*3+7 );
    cout << "tests completed ok" << endl;
}

