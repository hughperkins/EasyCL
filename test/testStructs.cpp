#include <iostream>
#include <cstdlib>
using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "CLKernel.h"

#include "test/asserts.h"

#include "CLKernel_structs.h"

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
    CLKernel *kernel = cl->buildKernel("../test/testStructs.cl", "testStructs");

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

