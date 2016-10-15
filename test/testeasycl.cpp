#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

static const char *getKernel();

using namespace easycl;

TEST(testeasycl, main) {
    cout << "start" << endl;
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");
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

