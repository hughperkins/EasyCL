#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"

#include "test/asserts.h"

using namespace std;
using namespace easycl;

TEST(testcopybuffer, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    //CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    float in[5];
    float in2[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3.0f;
        in2[i] = 23.0f + i;
    }
    float out[5];
    CLWrapper *inwrapper = cl->wrap(5, in);
    CLWrapper *in2wrapper = cl->wrap(5, in2);
    CLWrapper *outwrapper = cl->wrap(5, out);
    inwrapper->copyToDevice();
    in2wrapper->copyToDevice();

    EXPECT_FALSE(in2wrapper->isDeviceDirty());
    inwrapper->copyTo(in2wrapper);
    EXPECT_TRUE(in2wrapper->isDeviceDirty());
  //  cl->finish();
    // check that in2 host-side unchanged:
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3.0f;
        EXPECT_EQ(23.0f + i, in2[i]);
    }

    in2wrapper->copyToHost();
    // check that in2 is now a copy of in:
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3.0f;
        EXPECT_EQ(i * 3.0f, in2[i]);
    }

    // check that modifying in2 doesnt modfiy in:
    in2[1] = 27;
    in2wrapper->copyToDevice();
    inwrapper->copyToHost();
    EXPECT_EQ(1 * 3.0f, in[1]);

    in2wrapper->copyToHost();
    EXPECT_EQ(1 * 3.0f, in[1]);
    EXPECT_EQ(27.0f, in2[1]);
    
    delete inwrapper;
    delete in2wrapper;
    delete outwrapper;
    delete cl;
}

TEST(testcopybuffer, withoffset) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    //CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    float in[10];
    float in2[10];
    for(int i = 0; i < 10; i++) {
        in[i] = i * 3.0f;
        in2[i] = 23.0f + i;
    }
    float out[10];
    CLWrapper *inwrapper = cl->wrap(10, in);
    CLWrapper *in2wrapper = cl->wrap(10, in2);
    CLWrapper *outwrapper = cl->wrap(10, out);
    inwrapper->copyToDevice();
    in2wrapper->copyToDevice();

    EXPECT_FALSE(in2wrapper->isDeviceDirty());
    inwrapper->copyTo(in2wrapper, 2, 5, 4);
    EXPECT_TRUE(in2wrapper->isDeviceDirty());
  //  cl->finish();
    // check that in2 host-side unchanged:
    for(int i = 0; i < 10; i++) {
        in[i] = i * 3.0f;
        EXPECT_EQ(23.0f + i, in2[i]);
    }

    in2wrapper->copyToHost();
    // check that in2 is now a partial copy of in:
    for(int i = 0; i < 10; i++) {
//        in[i] = i * 3;
        if(i >= 5 && i < 9) {
          EXPECT_EQ((i-3) * 3.0f, in2[i]);
        } else {
          EXPECT_EQ(23.0f + i, in2[i]);
        }
    }

    // check that modifying in2 doesnt modfiy in:
    in2[1] = 27.0f;
    in2wrapper->copyToDevice();
    inwrapper->copyToHost();
    EXPECT_EQ(1 * 3.0f, in[1]);

    in2wrapper->copyToHost();
    EXPECT_EQ(1 * 3.0f, in[1]);
    EXPECT_EQ(27.0f, in2[1]);
    
    delete inwrapper;
    delete in2wrapper;
    delete outwrapper;
    delete cl;
}

TEST(SLOW_testcopybuffer, larger) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    //CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    const int bufferSize = 100 * 1024 * 1024 / 4; // 100MB (of floats)
    float *in = new float[bufferSize];
    float *in2 = new float[bufferSize];
    for(int i = 0; i < bufferSize; i++) {
        in[i] = i * 3.0f;
        in2[i] = 23.0f + i;
    }
    CLWrapper *inwrapper = cl->wrap(bufferSize, in);
    CLWrapper *in2wrapper = cl->wrap(bufferSize, in2);
    inwrapper->copyToDevice();
    in2wrapper->copyToDevice();

    inwrapper->copyTo(in2wrapper);
//    cl->finish();
    // check that in2 host-side unchanged:
    for(int i = 0; i < bufferSize; i++) {
        in[i] = i * 3.0f;
        EXPECT_EQ(23.0f + i, in2[i]);
    }

    in2wrapper->copyToHost();
    // check that in2 is now a copy of in:
    for(int i = 0; i < bufferSize; i++) {
        in[i] = i * 3.0f;
        EXPECT_EQ(i * 3.0f, in2[i]);
    }

    // check that modifying in2 doesnt modfiy in:
    in2[1] = 27.0f;
    in2wrapper->copyToDevice();
    inwrapper->copyToHost();
    EXPECT_EQ(1 * 3.0f, in[1]);

    in2wrapper->copyToHost();
    EXPECT_EQ(1 * 3.0f, in[1]);
    EXPECT_EQ(27.0f, in2[1]);
    
    delete inwrapper;
    delete in2wrapper;
    delete[] in;
    delete[] in2;
    delete cl;
}

TEST(testcopybuffer, throwsifnotondevice) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    //CLKernel *kernel = cl->buildKernel("testeasycl.cl", "test");
    const int bufferSize = 100 * 1024 / 4;
    float *in = new float[bufferSize];
    float *in2 = new float[bufferSize];
    for(int i = 0; i < bufferSize; i++) {
        in[i] = i * 3.0f;
        in2[i] = 23.0f + i;
    }
    CLWrapper *inwrapper = cl->wrap(bufferSize, in);
    CLWrapper *in2wrapper = cl->wrap(bufferSize, in2);
    inwrapper->copyToDevice();
//    in2wrapper->copyToDevice();

    bool threw = false;
    try {
        inwrapper->copyTo(in2wrapper);
    } catch(runtime_error &) {
        threw = true;
    }
    EXPECT_TRUE(threw);
    delete cl;
}

