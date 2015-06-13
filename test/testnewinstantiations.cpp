#include "EasyCL.h"
#include <iostream>
#include "DevicesInfo.h"
using namespace std;

#include "gtest/gtest.h"

void simpleTest( EasyCL *cl ) {
    CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    float in[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    float out[5];
    kernel->input( 5, in );
    kernel->output( 5, out );
    size_t global = 5;
    size_t local = 5;
    kernel->run(1, &global, &local );
    EXPECT_EQ( out[0] , 7 );
    EXPECT_EQ( out[1] , 10 );
    EXPECT_EQ( out[2] , 13 );
    EXPECT_EQ( out[3] , 16 );
    EXPECT_EQ( out[4] , 19 );
    delete kernel;
}

TEST( testnewinstantiations, createForFirstGpu ) {
    EasyCL *cl = EasyCL::createForFirstGpu();
    simpleTest( cl );
    delete cl;
}

TEST( testnewinstantiations, createForIndexedGpu ) {
    EasyCL *cl = EasyCL::createForIndexedGpu(0);
    simpleTest( cl );
    delete cl;
}

TEST( testnewinstantiations, createForIndexedDevice ) {
    int numDevices = easycl::DevicesInfo::getNumDevices();
    for( int i = 0; i < numDevices; i++ ) {
        EasyCL *cl = EasyCL::createForIndexedDevice(i);
        simpleTest( cl );
        delete cl;
    }
}

TEST( testnewinstantiations, createForPlatformDeviceIndexes ) {
    EasyCL *cl = EasyCL::createForPlatformDeviceIndexes(0, 0);
    simpleTest( cl );
    delete cl;
}

TEST( testnewinstantiations, createForFirstGpuOtherwiseCpu ) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    simpleTest( cl );
    delete cl;
}

