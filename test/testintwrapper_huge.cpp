#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"

//#include "unistd.h"

#include "test/asserts.h"
//#include "test/Timer.h"

using namespace std;
using namespace easycl;

static const char *getKernel();

TEST(SLOW_testintwrapper_huge, testreadnonwrapper) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test_read", "");
//    const int N = 4500000;
//    const int N = (4500000/512)*512;
    int N = 100000;
    int *out = new int[N];
    kernel->in(3)->in(7);
    kernel->output(N, out);
    int globalSize = N;
    int workgroupsize = cl->getMaxWorkgroupSize();
    globalSize = (( globalSize + workgroupsize - 1) / workgroupsize) * workgroupsize;
    cout << "globalsize: " << globalSize << " workgroupsize " << workgroupsize << endl;
//    Timer timer;
    kernel->run_1d(globalSize, workgroupsize);
//    timer.timeCheck("after kernel");
    for(int i = 0; i < 5; i++) {
           cout << "out[" << i << "]=" << out[i] << endl;
    }
    for(int i = 5; i < N; i++) {
       if(out[i] != 4228) {
           cout << "out[" << i << "] != 4228: " << out[i] << endl;
           exit(-1);
       }
    }

    delete[] out;
    delete kernel;
    delete cl;
}

TEST(SLOW_testintwrapper_huge, testread) {
//    Timer timer;
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test_read", "");
//    const int N = 4500000;
//    const int N = (4500000/512)*512;
    int N = 100000;
    int *out = new int[N];
    CLWrapper *outwrapper = cl->wrap(N, out);
    kernel->in(3)->in(7);
    kernel->output(outwrapper);
    int globalSize = N;
    int workgroupsize = cl->getMaxWorkgroupSize();
    globalSize = (( globalSize + workgroupsize - 1) / workgroupsize) * workgroupsize;
    cout << "globalsize: " << globalSize << " workgroupsize " << workgroupsize << endl;
//    timer.timeCheck("before kernel");
    kernel->run_1d(globalSize, workgroupsize);
//    timer.timeCheck("after kernel");
    outwrapper->copyToHost();
//    timer.timeCheck("after copy to host");
    for(int i = 0; i < N; i++) {
       if(out[i] != 4228) {
           cout << "out[" << i << "] != 4228: " << out[i] << endl;
           exit(-1);
       }
    }

    delete[] out;
    delete outwrapper;
    delete kernel;
    delete cl;
}

TEST(SLOW_testintwrapper_huge, testreadwrite) {

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test_stress", "");
    const int N = 1000000;
    int *in = new int[N];
    for(int i = 0; i < N; i++) {
        in[i] = i * 3;
    }
    int *out = new int[N];
    CLWrapper *inwrapper = cl->wrap(N, in);
    CLWrapper *outwrapper = cl->wrap(N, out);
    inwrapper->copyToDevice();
    outwrapper->createOnDevice();
    kernel->input(inwrapper);
    kernel->output(outwrapper);
    int globalSize = N;
    int workgroupsize = cl->getMaxWorkgroupSize();
    globalSize = (( globalSize + workgroupsize - 1) / workgroupsize) * workgroupsize;
    cout << "globalsize: " << globalSize << " workgroupsize " << workgroupsize << endl;
    kernel->run_1d(globalSize, workgroupsize);
    outwrapper->copyToHost();
    for(int i = 0; i < N; i++) {
       if(out[i] != 689514) {
           cout << "out[" << i << "] != 689514: " << out[i] << endl;
           exit(-1);
       }
    }

    delete[] out;
    delete outwrapper;
    delete inwrapper;
    delete kernel;
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testeasycl.cl")
    // ]]]
    // generated using cog, from test/testeasycl.cl:
    const char * source =  
    "kernel void test(global float *in, global float *out) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "    out[globalid] = in[globalid] + 7;\n" 
    "}\n" 
    "\n" 
    "kernel void testuchars(global unsigned char *in, global unsigned char *out) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "    out[globalid] = in[globalid] + 7;\n" 
    "}\n" 
    "\n" 
    "kernel void test_int(global int *in, global int *out) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "    out[globalid] = in[globalid] + 7;\n" 
    "}\n" 
    "\n" 
    "kernel void test_stress(global const int *in, global int *out) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "    int sum = 0;\n" 
    "    int n = 0;\n" 
    "   // make it do some work....\n" 
    "//    while(n < 1000000) {\n" 
    "    while(n < 10001) {\n" 
    "        sum = (sum + in[n % 47]) % (103070 * 7);\n" 
    "        n++;\n" 
    "    }\n" 
    "    out[globalid] = sum;\n" 
    "}\n" 
    "\n" 
    "kernel void test_read(const int one,  const int two, global int *out) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "    int sum = 0;\n" 
    "    int n = 0;\n" 
    "    while(n < 100000) {\n" 
    "        sum = (sum + one) % 1357 * two;\n" 
    "        n++;\n" 
    "    }\n" 
    "//    out[globalid+2048] = sum;\n" 
    "//    out[globalid] = sum;\n" 
    "//    out[0] = 44;\n" 
    "    out[globalid] = sum;\n" 
    "   // out[0] = globalid > out[0] ? globalid : out[0];\n" 
    "//    out[globalid] = 8827;\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

