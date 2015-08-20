#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

TEST(testinout, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testinout.cl", "test");
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

