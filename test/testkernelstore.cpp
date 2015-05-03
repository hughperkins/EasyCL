#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "gtest/gtest.h"
//#include "test/asserts.h"

TEST( testkernelstore, main ) {
    if( !EasyCL::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testfloatarray.cl", "test");
    EXPECT_EQ( false, cl->kernelExists("kernela") );
    EXPECT_EQ( false, cl->kernelExists("kernelb") );

    cl->storeKernel( "kernela", kernel );
    EXPECT_EQ( true, cl->kernelExists("kernela") );
    EXPECT_EQ( false, cl->kernelExists("kernelb") );

    EXPECT_EQ( kernel, cl->getKernel("kernela") );

    delete kernel;
    delete cl;
}

