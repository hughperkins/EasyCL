#include "EasyCL.h"

#include "gtest/gtest.h"

#include <iostream>
#include <stdexcept>
using namespace std;

static const char *getKernel();

using namespace easycl;

TEST(testbuildlog, main) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    try {
        cl->buildKernelFromString(getKernel(), "foo", "", "testbuildlog.cl");
    } catch(std::runtime_error err) {
        cout << err.what() << endl;
        EXPECT_TRUE(string(err.what()).find("testbuildlog.cl") != string::npos);
    }
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testbuildlog.cl")
    // ]]]
    // generated using cog, from test/testbuildlog.cl:
    const char * source =  
    "kernel void foo() {\n" 
    "   someerrorxyz;\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

