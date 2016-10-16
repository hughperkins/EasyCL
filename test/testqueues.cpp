// Copyright Hugh Perkins 2013, 2014, 2015, 2016 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

static const char *getKernel();

using namespace easycl;

TEST(testqueues, main) {
    cout << "start" << endl;
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLQueue *queue2 = cl->newQueue();
    CLQueue *queue3 = cl->newQueue();

    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");
    // CLKernel *kernel2 = cl->buildKernelFromString(queue, getKernel(), "test", "");

    float in[5];
    float out[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
        out[i] = 0;
    }
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

    // in[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
        out[i] = 0;
    }
    // float out[5];
    kernel->input(5, in);
    kernel->output(5, out);
    // size_t global = 5;
    // size_t local = 5;
    kernel->run(queue2, 1, &global, &local);
    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);

    // in[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
        out[i] = 0;
    }
    // float out[5];
    kernel->input(5, in);
    kernel->output(5, out);
    // size_t global = 5;
    // size_t local = 5;
    kernel->run(queue3, 1, &global, &local);
    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);

    // in[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
        out[i] = 0;
    }
    // float out[5];
    kernel->input(5, in);
    kernel->output(5, out);
    // size_t global = 5;
    // size_t local = 5;
    kernel->run(1, &global, &local);
    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);

    // in[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
        out[i] = 0;
    }
    // float out[5];
    kernel->input(5, in);
    kernel->output(5, out);
    // size_t global = 5;
    // size_t local = 5;
    kernel->run(queue2, 1, &global, &local);
    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);

    delete queue2;
    delete queue3;

    cout << "tests completed ok" << endl;
}

TEST(testqueues, defaultqueue) {
    cout << "start" << endl;
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");

    float in[5];
    float out[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
        out[i] = 0;
    }
    kernel->input(5, in);
    kernel->output(5, out);
    size_t global = 5;
    size_t local = 5;
    kernel->run(1, &global, &local);
    cl->finish();
    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);

    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
        out[i] = 0;
    }

    cout << "cl->queue " << (void *)cl->queue << endl;
    cout << "*cl->queue " << (void *)*cl->queue << endl;
    cout << "cl->default_queue->queue" << (void *)cl->default_queue->queue << endl;
    kernel->input(5, in);
    kernel->output(5, out);
    kernel->run(cl->default_queue, 1, &global, &local);
    cl->finish();
    assertEquals(out[0] , 7);
    assertEquals(out[1] , 10);
    assertEquals(out[2] , 13);
    assertEquals(out[3] , 16);
    assertEquals(out[4] , 19);

    cout << "tests completed ok" << endl;
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
