#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"

#include "test/asserts.h"

using namespace std;

static const char *getKernel();

TEST(testworkgroupsize, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");
    int maxWorkgroupSize = cl->getMaxWorkgroupSize();
    cout << "max workgroup size: " << maxWorkgroupSize << endl;
    for(int workgroupSize = 16; workgroupSize <= maxWorkgroupSize; workgroupSize <<= 1 ) {
        float *in = new float[workgroupSize];
        float *out = new float[workgroupSize];
        for(int i = 0; i < workgroupSize; i++) {
            in[i] = i * 3;
        }
        CLWrapper *inwrapper = cl->wrap(workgroupSize, in);
        CLWrapper *outwrapper = cl->wrap(workgroupSize, out);
        inwrapper->copyToDevice();
        kernel->input(inwrapper);
        kernel->output(outwrapper);
        kernel->run_1d(workgroupSize, workgroupSize);
        outwrapper->copyToHost();
        for(int i = 0; i < workgroupSize; i+= (workgroupSize / 8)) {
            assertEquals(out[i] , (i + 2) * 3 + 1);
        }
        cout << "tests completed ok for workgroupSize " << workgroupSize << endl;
        delete[] in;
        delete[] out;
        delete inwrapper;
        delete outwrapper;
    }

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

