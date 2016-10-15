#include "EasyCL.h"
#include <iostream>
#include "DevicesInfo.h"
using namespace std;

#include "gtest/gtest.h"

static const char *getKernel();

using namespace easycl;

void simpleTest(EasyCL *cl) {
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
    EXPECT_EQ(out[0] , 7);
    EXPECT_EQ(out[1] , 10);
    EXPECT_EQ(out[2] , 13);
    EXPECT_EQ(out[3] , 16);
    EXPECT_EQ(out[4] , 19);
    delete kernel;
}

TEST(testnewinstantiations, createForFirstGpu) {
    EasyCL *cl = EasyCL::createForFirstGpu();
    simpleTest(cl);
    delete cl;
}

TEST(testnewinstantiations, createForIndexedGpu) {
    EasyCL *cl = EasyCL::createForIndexedGpu(0);
    simpleTest(cl);
    delete cl;
}

TEST(testnewinstantiations, createForIndexedDevice) {
    int numDevices = easycl::DevicesInfo::getNumDevices();
    for(int i = 0; i < numDevices; i++) {
        EasyCL *cl = EasyCL::createForIndexedDevice(i);
        simpleTest(cl);
        delete cl;
    }
}

TEST(testnewinstantiations, createForPlatformDeviceIndexes) {
    EasyCL *cl = EasyCL::createForPlatformDeviceIndexes(0, 0);
    simpleTest(cl);
    delete cl;
}

TEST(testnewinstantiations, createForFirstGpuOtherwiseCpu) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    simpleTest(cl);
    delete cl;
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

