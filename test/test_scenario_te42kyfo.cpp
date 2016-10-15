#include <iostream>

#include "gtest/gtest.h"

#include "EasyCL.h"
#include "test/asserts.h"

using namespace std;
using namespace easycl;

static const char *getKernel();

TEST(test_scenario_te42kyfo, main) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "test", "");
    CLArrayFloat *out = cl->arrayFloat(5);
    CLArrayFloat *in = cl->arrayFloat(5);
    for(int i = 0; i < 5; i++) {
       (*out)[i] = 0;
    }
    for(int i = 0; i < 100; i++) {
      for(int n = 0; n < 5; n++) {
        (*in)[n] = i*n;
      }
      kernel->in(in);
      kernel->out(out);
      kernel->run_1d(5, 5);
      assertEquals(i*2 + 5, (*out)[2]);
      assertEquals(i*4 + 5, (*out)[4]);
    }
    cout << "finished" << endl;

    delete in;
    delete out;
    delete kernel;
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/test_scenario_te42kyfo.cl")
    // ]]]
    // generated using cog, from test/test_scenario_te42kyfo.cl:
    const char * source =  
    "kernel void test(global float *one, global float *two) {\n" 
    "    two[get_global_id(0)] = one[get_global_id(0)] + 5;\n" 
    "}\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

