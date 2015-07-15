#include "EasyCL.h"
#include <iostream>
#include <cstdlib>

#include "gtest/gtest.h"
#include "test/asserts.h"

using namespace std;

static const char *kernelSource = 
"kernel void test(int N, global float *in){\n"
"    if( get_global_id(0) >= N ) {\n"
"        return;\n"
"    }\n"
"    in[get_global_id(0)] += 1.0f;\n"
"}\n"
;

TEST( testprofiling, basic ) {
    if( !EasyCL::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    cl->setProfiling(true);
    CLKernel *kernel = cl->buildKernelFromString(kernelSource, "test", "", "");
    const int N = 8 * 1024 * 1024;
    float *in = new float[N];
    for( int i = 0; i < N; i++ ) {
        in[i] = i * 3;
    }
    CLWrapper *inwrapper = cl->wrap(N, in);
    inwrapper->copyToDevice();
    const int its = 16;
    for( int i = 0 ; i < its; i++ ) {
      kernel->in(N);
      kernel->inout( inwrapper );
      kernel->run_1d( ((N+64-1)/64) * 64, 64 );
    }
    cl->dumpProfiling();
    inwrapper->copyToHost();

    delete inwrapper;
    delete[] in;
    delete kernel;
    delete cl;
}

