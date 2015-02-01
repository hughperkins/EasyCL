#include <iostream>

#include "gtest/gtest.h"

#include "OpenCLHelper.h"
#include "test/asserts.h"

using namespace std;

TEST( test_scenario_te42kyfo, main ) {
    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("test_scenario_te42kyfo.cl", "test");
    CLArrayFloat *out = cl->arrayFloat(5);
    CLArrayFloat *in = cl->arrayFloat(5);
    for( int i = 0; i < 5; i++ ) {
       (*out)[i] = 0;
    }
    for(int i = 0; i < 100; i++) {
      for( int n = 0; n < 5; n++) {
        (*in)[n] = i*n;
      }
      kernel->in( in );
      kernel->out( out );
      kernel->run_1d( 5, 5 );
      assertEquals( i*2 + 5, (*out)[2] );
      assertEquals( i*4 + 5, (*out)[4] );
    }
    cout << "finished" << endl;

    delete in;
    delete out;
    delete kernel;
    delete cl;
}

