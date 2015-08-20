#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "gtest/gtest.h"
//#include "test/asserts.h"

TEST(testkernelstore, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testfloatarray.cl", "test");
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
    CLKernel *kernela = cl->buildKernel("testfloatarray.cl", "test");
    CLKernel *kernelb = cl->buildKernel("testfloatarray.cl", "test");
    CLKernel *kernelc = cl->buildKernel("testfloatarray.cl", "test");
    CLKernel *kerneld = cl->buildKernel("testfloatarray.cl", "test");
    cl->storeKernel("kernela", kernela);
    cl->storeKernel("kernelb", kernelb, true);
    cl->storeKernel("kernelc", kernelc, false);
    cl->storeKernel("kerneld", kerneld, true);

    delete kernela;
    delete kernelc;

    delete cl;
}

