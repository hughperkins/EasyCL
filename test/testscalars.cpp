#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

static const char *getKernel();

using namespace easycl;

TEST(testscalars, test1) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");
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

    for(int i = 0; i < 5; i++) {
        EXPECT_FLOAT_NEAR(1.0f + i + 0.234f, floatout[i]);
        // EXPECT_TRUE(std::abs(floatout[i] - i - 0.234f) <= 1e-4f);
    }
    cout << "tests completed ok" << endl;

    delete kernel;
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testscalars.cl")
    // ]]]
    // generated using cog, from test/testscalars.cl:
    const char * source =  
    "kernel void test(const int aint, const unsigned auint,\n" 
    "    long along, unsigned long aulong,\n" 
    "  const float afloat,\n" 
    "  global int *intout,\n" 
    "  global unsigned int *uintout,\n" 
    "  global long *longout,\n" 
    "  global unsigned long *ulongout,\n" 
    "  global float *floatout\n" 
    " ) {\n" 
    "    const int globalid = get_global_id(0);\n" 
    "\n" 
    "    intout[globalid] = aint + globalid;\n" 
    "    uintout[globalid] = auint + globalid;\n" 
    "    longout[globalid] = along + globalid;\n" 
    "    ulongout[globalid] = aulong + globalid;\n" 
    "\n" 
    "    floatout[globalid] = afloat + globalid;\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

