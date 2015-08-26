#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

TEST(testscalars, test1) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testscalars.cl", "test");
    int intout[5];
    unsigned int uintout[5];
    int64_t int64out[5];
    uint64_t uint64out[5];
    float floatout[5];
    kernel->in(-156);
    kernel->in((unsigned int)3000);
    kernel->in((int64_t)-2524653l);
    kernel->in((uint64_t)1353523545l);
    kernel->in(1.234f);
    kernel->out(5, intout);
    kernel->out(5, uintout);
    kernel->out(5, int64out);
    kernel->out(5, uint64out);
    kernel->out(5, floatout);
    kernel->run_1d(5, 5);

    for(int i = 0; i < 5; i++) {
        cout << intout[i] << " ";
    }
    cout << endl;
    for(int i = 0; i < 5; i++) {
        cout << uintout[i] << " ";
    }
    cout << endl;
    for(int i = 0; i < 5; i++) {
        cout << int64out[i] << " ";
    }
    cout << endl;
    for(int i = 0; i < 5; i++) {
        cout << uint64out[i] << " ";
    }
    cout << endl;
    for(int i = 0; i < 5; i++) {
        cout << floatout[i] << " ";
    }
    cout << endl;

    assertEquals(intout[0], -156);
    assertEquals(intout[1] , -155);
    assertEquals(intout[2] , -154);
    assertEquals(intout[3] , -153);
    assertEquals(intout[4], -152);

    assertEquals(uintout[2] , (unsigned int)3002);
    assertEquals(int64out[2] ,  (int64_t)(-2524653ll) + 2);
    assertEquals(uint64out[2] , (uint64_t)(1353523545ll + 2));

    assertEquals(floatout[0] , 1.234f);
    assertEquals(floatout[1] , 2.234f);
    assertEquals(floatout[2] , 3.234f);
    assertEquals(floatout[3] , 4.234f);
    assertEquals(floatout[4] , 5.234f);
    cout << "tests completed ok" << endl;

    delete kernel;
    delete cl;
}

