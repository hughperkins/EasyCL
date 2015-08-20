#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "EasyCL.h"

#include "test/asserts.h"

using namespace std;

#define uc (unsigned char)

TEST(testucharwrapper, main) {
    if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testeasycl.cl", "testuchars");
    unsigned char in[5];
    for(int i = 0; i < 5; i++) {
        in[i] = i * 3;
    }
    unsigned char out[5];
    CLWrapper *inwrapper = cl->wrap(5, in);
    CLWrapper *outwrapper = cl->wrap(5, out);
    inwrapper->copyToDevice();
    kernel->input(inwrapper);
    kernel->output(outwrapper);
    kernel->run_1d(5, 5);
    outwrapper->copyToHost();
    assertEquals(out[0] , uc 7);
    assertEquals(out[1] , uc 10);
    assertEquals(out[2] , uc 13);
    assertEquals(out[3] , uc 16);
    assertEquals(out[4] , uc 19);
    cout << "tests completed ok" << endl;

    delete inwrapper;
    delete outwrapper;
    delete kernel;
    delete cl;
}

