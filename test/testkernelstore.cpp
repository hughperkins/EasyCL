#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "gtest/gtest.h"
//#include "test/asserts.h"

static const char *getKernel();

using namespace easycl;

TEST(testkernelstore, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");
    EXPECT_FALSE(cl->kernelExists("kernela"));
    EXPECT_FALSE(cl->kernelExists("kernelb"));

    cl->storeKernel("kernela", kernel);
    EXPECT_TRUE(cl->kernelExists("kernela"));
    EXPECT_FALSE(cl->kernelExists("kernelb"));

    EXPECT_EQ(kernel, cl->getKernel("kernela"));

    delete kernel;
    delete cl;
}

TEST(testkernelstore, cl_deletes) {
    // checks the new deleteWithCl parameter
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernela = cl->buildKernelFromString(getKernel(), "test", "");
    CLKernel *kernelb = cl->buildKernelFromString(getKernel(), "test", "");
    CLKernel *kernelc = cl->buildKernelFromString(getKernel(), "test", "");
    CLKernel *kerneld = cl->buildKernelFromString(getKernel(), "test", "");
    cl->storeKernel("kernela", kernela);
    cl->storeKernel("kernelb", kernelb, true);
    cl->storeKernel("kernelc", kernelc, false);
    cl->storeKernel("kerneld", kerneld, true);

    delete kernela;
    delete kernelc;

    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testfloatarray.cl")
    // ]]]
    // generated using cog, from test/testfloatarray.cl:
    const char * source =  
    "kernel void test(global float *in, global float *out, global float *inout) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "    inout[globalid] = inout[globalid] + 7;\n" 
    "    out[globalid] = in[globalid] + 5;\n" 
    "    if(globalid == 2) {\n" 
    "        out[globalid] = 26;\n" 
    "        inout[globalid] = 34;\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

