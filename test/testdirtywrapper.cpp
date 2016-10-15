// Copyright Hugh Perkins 2013, 2014, 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"

#include "test/asserts.h"

using namespace std;
using namespace easycl;

static const char *getKernel();

TEST(testdirtywrapper, main) {
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
    CLWrapper *inwrapper = cl->wrap(5, in);
    CLWrapper *outwrapper = cl->wrap(5, out);
    EXPECT_FALSE(inwrapper->isDeviceDirty());
    EXPECT_FALSE(outwrapper->isDeviceDirty());

    inwrapper->copyToDevice();
    EXPECT_FALSE(inwrapper->isDeviceDirty());
    EXPECT_FALSE(outwrapper->isDeviceDirty());

    kernel->input(inwrapper);
    kernel->output(outwrapper);
    EXPECT_FALSE(inwrapper->isDeviceDirty());
    EXPECT_FALSE(outwrapper->isDeviceDirty());

    kernel->run_1d(5, 5);
    EXPECT_FALSE(inwrapper->isDeviceDirty());
    EXPECT_TRUE(outwrapper->isDeviceDirty());

    outwrapper->copyToHost();
    EXPECT_FALSE(outwrapper->isDeviceDirty());

    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);
    cout << "tests completed ok" << endl;

    delete inwrapper;
    delete outwrapper;
    delete kernel;
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

