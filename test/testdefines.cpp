#include <iostream>

#include "gtest/gtest.h"

#include "EasyCL.h"

using namespace std;
using namespace easycl;

static const char *getKernel();

TEST(testdefines, simple) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "testDefines", 
        "-D DOUBLE -D SOME_VALUE=5");
    float out[32];
    kernel->out(32, out);
    kernel->run_1d(32, 32);
    EXPECT_EQ(10, out[3]);

    delete kernel;
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testdefines.cl")
    // ]]]
    // generated using cog, from test/testdefines.cl:
    const char * source =  
    "kernel void testDefines(global float *out) {\n" 
    "    int globalId = get_global_id(0);\n" 
    "#ifdef DOUBLE\n" 
    "    out[globalId] = SOME_VALUE * 2;\n" 
    "#else\n" 
    "    out[globalId] = SOME_VALUE;\n" 
    "#endif\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

