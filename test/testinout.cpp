#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

static const char *getKernel();

using namespace easycl;

TEST(testinout, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");
    float inout[5];
    for(int i = 0; i < 5; i++) {
        inout[i] = i * 3;
    }
    kernel->inout(5, inout);
    size_t global = 5;
    size_t local = 5;
    kernel->run(1, &global, &local);
    assertEquals(inout[0] , 7);
    assertEquals(inout[1] , 10);
    assertEquals(inout[2] , 13);
    assertEquals(inout[3] , 16);
    assertEquals(inout[4] , 19);
    cout << "tests completed ok" << endl;

    delete kernel;
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testinout.cl")
    // ]]]
    // generated using cog, from test/testinout.cl:
    const char * source =  
    "kernel void test(global float *inout) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "    inout[globalid] = inout[globalid] + 7;\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

