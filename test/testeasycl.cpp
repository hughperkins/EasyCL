#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

TEST(testeasycl, main) {
    cout << "start" << endl;
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    float in[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
    }
    float out[5];
    kernel->input(5, in);
    kernel->output(5, out);
    size_t global = 5;
    size_t local = 5;
    kernel->run(1, &global, &local);
    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);
    cout << "tests completed ok" << endl;
}

TEST(testeasycl, power2helper) {
    ASSERT_EQ(1, EasyCL::getPower2Upperbound(1) );
    ASSERT_EQ(2, EasyCL::getPower2Upperbound(2) );
    ASSERT_EQ(4, EasyCL::getPower2Upperbound(3) );
    ASSERT_EQ(4, EasyCL::getPower2Upperbound(4) );
    ASSERT_EQ(8, EasyCL::getPower2Upperbound(5) );
    ASSERT_EQ(8, EasyCL::getPower2Upperbound(6) );
    ASSERT_EQ(8, EasyCL::getPower2Upperbound(7) );
    ASSERT_EQ(8, EasyCL::getPower2Upperbound(8) );
    ASSERT_EQ(16, EasyCL::getPower2Upperbound(9) );
    ASSERT_EQ(16, EasyCL::getPower2Upperbound(15) );
    ASSERT_EQ(16, EasyCL::getPower2Upperbound(16) );
    ASSERT_EQ(32, EasyCL::getPower2Upperbound(17) );

    ASSERT_EQ(1, EasyCL::getNextPower2(1) );
    ASSERT_EQ(2, EasyCL::getNextPower2(2) );
    ASSERT_EQ(4, EasyCL::getNextPower2(3) );
    ASSERT_EQ(4, EasyCL::getNextPower2(4) );
    ASSERT_EQ(8, EasyCL::getNextPower2(5) );
    ASSERT_EQ(8, EasyCL::getNextPower2(6) );
    ASSERT_EQ(8, EasyCL::getNextPower2(7) );
    ASSERT_EQ(8, EasyCL::getNextPower2(8) );
    ASSERT_EQ(16, EasyCL::getNextPower2(9) );
    ASSERT_EQ(16, EasyCL::getNextPower2(15) );
    ASSERT_EQ(16, EasyCL::getNextPower2(16) );
    ASSERT_EQ(32, EasyCL::getNextPower2(17) );
}


