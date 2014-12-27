#include <iostream>

#include "OpenCLHelper.h"

using namespace std;

int main( int argc, char *argv[] ) {
    OpenCLHelper cl;
    CLKernel *kernel = cl.buildKernel("../test/test_scenario_te42kyfo.cl", "test");
    CLArrayFloat *out = cl.arrayFloat(5);
    CLArrayFloat *in = cl.arrayFloat(5);
    for( int i = 0; i < 5; i++ ) {
       (*out)[i] = 0;
    }
    for(int i = 0; i < 100; i++) {
      for( int n = 0; n < 5; n++) {
        (*in)[n] = i*n;
      }
      cout << "i " << i << endl;
      kernel->in( in );
      kernel->out( out );
      kernel->run_1d( 5, 5 );
    }
}

