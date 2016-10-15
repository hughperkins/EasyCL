#include "EasyCL.h"
#include <iostream>
#include <cstdlib>
#include "util/StatefulTimer.h"

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

TEST(teststatefultimer, basic) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    StatefulTimer::setEnabled(true);
    CLKernel *kernel = cl->buildKernelFromString(kernelSource, "test", "", "source1");
    const int N = 8 * 1024 * 1024;
    float *in = new float[N];
    for(int i = 0; i < N; i++) {
        in[i] = i * 3;
    }
    CLWrapper *inwrapper = cl->wrap(N, in);
    inwrapper->copyToDevice();
    cl->finish();
    StatefulTimer::dump(true);
    double start = StatefulTimer::getSystemMilliseconds();
    StatefulTimer::timeCheck("start");

    const int its = 16;
    for(int i = 0 ; i < its; i++) {
      kernel->in(N);
      kernel->inout(inwrapper);
      kernel->run_1d(((N+64-1)/64) * 64, 64);
      StatefulTimer::timeCheck("foo");
    }
    cl->finish();
    StatefulTimer::timeCheck("end");
    double end = StatefulTimer::getSystemMilliseconds();
    StatefulTimer::dump(true);
    cout << "time: " << (end-start) << endl;
    inwrapper->copyToHost();

    StatefulTimer::setEnabled(false);

    delete inwrapper;
    delete[] in;
    delete kernel;
    delete cl;
}

TEST(teststatefultimer, notiming) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
//    cl->setProfiling(true);
    CLKernel *kernel = cl->buildKernelFromString(kernelSource, "test", "", "source1");
    const int N = 8 * 1024 * 1024;
    float *in = new float[N];
    for(int i = 0; i < N; i++) {
        in[i] = i * 3;
    }
    CLWrapper *inwrapper = cl->wrap(N, in);
    inwrapper->copyToDevice();
    cl->finish();
    StatefulTimer::dump(true);
    double start = StatefulTimer::getSystemMilliseconds();
    const int its = 16;
    for(int i = 0 ; i < its; i++) {
      kernel->in(N);
      kernel->inout(inwrapper);
      kernel->run_1d(((N+64-1)/64) * 64, 64);
      StatefulTimer::timeCheck("foo");
    }
    cl->finish();
    double end = StatefulTimer::getSystemMilliseconds();
    StatefulTimer::dump(true);
    cout << "time: " << (end-start) << endl;
    inwrapper->copyToHost();

    delete inwrapper;
    delete[] in;
    delete kernel;
    delete cl;
}

