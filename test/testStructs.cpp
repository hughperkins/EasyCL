#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

#include "CLKernel_structs.h"

static const char *getKernel();

using namespace easycl;

typedef struct MyStruct {
    int anint;
    float afloat;
    int threeints[3];
} MyStruct;

// template EasyCL_EXPORT CLKernel *CLKernel::inout(int N, MyStruct *data);

TEST(testStructs, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "testStructs", "");

    int ints[3];
    float floats[3];
    MyStruct mystruct;
    mystruct.anint = 13;
    mystruct.afloat = 1.234f;
    mystruct.threeints[1] = 3;
    mystruct.threeints[2] = 6;
    kernel->inout(3, ints)->inout(3, floats)->inout(1, &mystruct);
    kernel->run_1d(16, 16);
    cl->finish();

    cout << "ints[0] " << ints[0] << endl;
    cout << "floats[0] " << floats[0] << endl;
    cout << "mystruct.threeints[0]=" << mystruct.threeints[0] << endl;

    delete kernel;
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testStructs.cl")
    // ]]]
    // generated using cog, from test/testStructs.cl:
    const char * source =  
    "typedef struct MyStruct {\n" 
    "    int anint;\n" 
    "    float afloat;\n" 
    "    int threeints[3];\n" 
    "} MyStruct;\n" 
    "\n" 
    "kernel void testStructs(global int *ints, global float *floats, global MyStruct *structs) {\n" 
    "    if(get_global_id(0) != 0) {\n" 
    "        return;\n" 
    "    }\n" 
    "    floats[0] = structs[0].afloat;\n" 
    "    ints[0] = structs[0].anint;\n" 
    "    structs[0].threeints[0] = structs[0].threeints[1] + structs[0].threeints[2];\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

