#include "EasyCL.h"
#include <iostream>
#include <cstdlib>

#include "gtest/gtest.h"
#include "test/asserts.h"

using namespace std;
using namespace easycl;

static const char *kernelSource = 
"kernel void test(int N, global float *in){\n"
"    if((int)get_global_id(0) >= N) {\n"
"        return;\n"
"    }\n"
"    in[get_global_id(0)] += 1.0f;\n"
"}\n"
;

TEST(testprofiling, basic) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    cl->setProfiling(true);
    CLKernel *kernel = cl->buildKernelFromString(kernelSource, "test", "", "source1");
    CLKernel *kernel2 = cl->buildKernelFromString(kernelSource, "test", "", "source2");
    CLKernel *kernel3 = cl->buildKernelFromString(kernelSource, "test", "", "source3");
    CLKernel *kernel4 = cl->buildKernelFromString(kernelSource, "test", "", "source4");
    const int N = 8 * 1024 * 1024;
    float *in = new float[N];
    for(int i = 0; i < N; i++) {
        in[i] = i * 3.0f;
    }
    CLWrapper *inwrapper = cl->wrap(N, in);
    inwrapper->copyToDevice();
    const int its = 16;
    for(int i = 0 ; i < its; i++) {
      kernel->in(N);
      kernel->inout(inwrapper);
      kernel->run_1d(((N+64-1)/64) * 64, 64);
      kernel2->in(N/8);
      kernel2->inout(inwrapper);
      kernel2->run_1d(((N/8+64-1)/64) * 64, 64);
      kernel3->in(N/64);
      kernel3->inout(inwrapper);
      kernel3->run_1d(((N/64+64-1)/64) * 64, 64);
      kernel4->in(N/64/8);
      kernel4->inout(inwrapper);
      kernel4->run_1d(((N/64/8+64-1)/64) * 64, 64);
    }
    cl->dumpProfiling();
    inwrapper->copyToHost();

    delete inwrapper;
    delete[] in;
    delete kernel;
    delete kernel2;
    delete kernel3;
    delete kernel4;
    delete cl;
}

TEST(testprofiling, noprofiling) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
//    cl->setProfiling(true);
    CLKernel *kernel = cl->buildKernelFromString(kernelSource, "test", "", "source1");
    const int N = 2048;
    float *in = new float[N];
    for(int i = 0; i < N; i++) {
        in[i] = i * 3.0f;
    }
    CLWrapper *inwrapper = cl->wrap(N, in);
    inwrapper->copyToDevice();
    const int its = 16;
    for(int i = 0 ; i < its; i++) {
      kernel->in(N);
      kernel->inout(inwrapper);
      kernel->run_1d(((N+64-1)/64) * 64, 64);
    }
    cl->dumpProfiling();
    inwrapper->copyToHost();

    delete inwrapper;
    delete[] in;
    delete kernel;
    delete cl;
}

