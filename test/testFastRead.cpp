#include <iostream>
#include <cstdlib>
#include <chrono>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

static const char *getKernel();

using namespace easycl;

void testManyOut(CLKernel *kernel, size_t workGroupSize, bool fast_read){
    float in[workGroupSize];
    for(int i = 0; i < workGroupSize; i++) {
        in[i] = i;
    }
    kernel->in(workGroupSize, in);

    float out1[workGroupSize];
    float out2[workGroupSize];
    float out3[workGroupSize];
    float out4[workGroupSize];
    float out5[workGroupSize];
    float out6[workGroupSize];
    kernel->out(workGroupSize, out1);
    kernel->out(workGroupSize, out2);
    kernel->out(workGroupSize, out3);
    kernel->out(workGroupSize, out4);
    kernel->out(workGroupSize, out5);
    kernel->out(workGroupSize, out6);



    size_t global = workGroupSize;
    size_t local = workGroupSize;
    kernel->run(1, &global, &local, fast_read);

    for(int i=0; i<workGroupSize; i++){
	assertEquals(out1[i], i+1);
    	assertEquals(out2[i], i+3);
    	assertEquals(out3[i], i+51);
    	assertEquals(out4[i], i+9);
    	assertEquals(out5[i], i+11);
    	assertEquals(out6[i], i+186);
    }
}

TEST(testFastRead, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");


    size_t workGroupSize=0;
    cl_int error = clGetKernelWorkGroupInfo(kernel->kernel, cl->device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &workGroupSize, NULL);

    if(error!=CL_SUCCESS){
        switch(error){
            case CL_INVALID_DEVICE:
                std::cout << "CL_INVALID_DEVICE" << std::endl;
                break;
            case CL_INVALID_VALUE:
                std::cout << "CL_INVALID_VALUE" << std::endl;
                break;
            case CL_INVALID_KERNEL:
                std::cout << "CL_INVALID_KERNEL" << std::endl;
                break;
            case CL_OUT_OF_RESOURCES:
                std::cout << "CL_OUT_OF_RESOURCES" << std::endl;
                break;
        }
    }
    std::cout << "CL_KERNEL_WORK_GROUP_SIZE: " << workGroupSize << std::endl;

    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;

    int warmupCycles = 300;
    int testCycles = 500;
    for(int i=0; i<warmupCycles; i++){
        testManyOut(kernel, workGroupSize, false);
    }

    start = std::chrono::steady_clock::now();
    for(int i=0; i<testCycles; i++){
        testManyOut(kernel, workGroupSize, false);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "Non fast_read took:  " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/testCycles << "us." << std::endl;

    for(int i=0; i<warmupCycles; i++){
        testManyOut(kernel, workGroupSize, true);
    }

    start = std::chrono::steady_clock::now();
    for(int i=0; i<testCycles; i++){
        testManyOut(kernel, workGroupSize, true);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "Fast_read took:  " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/testCycles << "us." << std::endl;

    cout << "tests completed ok" << endl;

    delete kernel;
    delete cl;
}



static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testinout.cl")
    // ]]]
    // generated using cog, from test/testinout.cl:
    const char * source =
    "kernel void test(global float *in, global float *out1, global float *out2, global float *out3, global float *out4, global float *out5, global float *out6) {\n"
    "    const int globalid = get_global_id(0);\n"
    "    out1[globalid] = in[globalid] + 1;\n"
    "    out2[globalid] = in[globalid] + 3;\n"
    "    out3[globalid] = in[globalid] + 51;\n"
    "    out4[globalid] = in[globalid] + 9;\n"
    "    out5[globalid] = in[globalid] + 11;\n"
    "    out6[globalid] = in[globalid] + 186;\n"
    "}\n"
    "\n"
    "";
    // [[[end]]]
    return source;
}
