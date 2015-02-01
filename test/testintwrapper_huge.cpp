#include <iostream>
#include <cstdlib>
// using namespace std;

#include "gtest/gtest.h"

#include "OpenCLHelper.h"

//#include "unistd.h"

#include "test/asserts.h"
#include "test/Timer.h"

using namespace std;

TEST( SLOW_testintwrapper_huge, testreadnonwrapper ) {
    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testopenclhelper.cl", "test_read");
//    const int N = 4500000;
//    const int N = (4500000/512)*512;
    int N = 100000;
    int *out = new int[N];
    kernel->in(3)->in(7);
    kernel->output( N, out );
    int globalSize = N;
    int workgroupsize = cl->getMaxWorkgroupSize();
    globalSize = ( ( globalSize + workgroupsize - 1 ) / workgroupsize ) * workgroupsize;
    cout << "globalsize: " << globalSize << " workgroupsize " << workgroupsize << endl;
    Timer timer;
    kernel->run_1d( globalSize, workgroupsize );
    timer.timeCheck("after kernel");
    for( int i = 0; i < 5; i++ ) {
           cout << "out[" << i << "]=" << out[i] << endl;
    }
    for( int i = 5; i < N; i++ ) {
       if( out[i] != 4228 ) {
           cout << "out[" << i << "] != 4228: " << out[i] << endl;
           exit(-1);
       }
    }

    delete kernel;
    delete cl;
}

TEST( SLOW_testintwrapper_huge, testread ) {
    Timer timer;
    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testopenclhelper.cl", "test_read");
//    const int N = 4500000;
//    const int N = (4500000/512)*512;
    int N = 100000;
    int *out = new int[N];
    CLWrapper *outwrapper = cl->wrap(N, out);
    kernel->in(3)->in(7);
    kernel->output( outwrapper );
    int globalSize = N;
    int workgroupsize = cl->getMaxWorkgroupSize();
    globalSize = ( ( globalSize + workgroupsize - 1 ) / workgroupsize ) * workgroupsize;
    cout << "globalsize: " << globalSize << " workgroupsize " << workgroupsize << endl;
    timer.timeCheck("before kernel");
    kernel->run_1d( globalSize, workgroupsize );
    timer.timeCheck("after kernel");
    outwrapper->copyToHost();
    timer.timeCheck("after copy to host");
    for( int i = 0; i < N; i++ ) {
       if( out[i] != 4228 ) {
           cout << "out[" << i << "] != 4228: " << out[i] << endl;
           exit(-1);
       }
    }

    delete outwrapper;
    delete kernel;
    delete cl;
}

TEST( SLOW_testintwrapper_huge, testreadwrite ) {

    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testopenclhelper.cl", "test_stress");
    const int N = 1000000;
    int *in = new int[N];
    for( int i = 0; i < N; i++ ) {
        in[i] = i * 3;
    }
    int *out = new int[N];
    CLWrapper *inwrapper = cl->wrap(N, in);
    CLWrapper *outwrapper = cl->wrap(N, out);
    inwrapper->copyToDevice();
    outwrapper->createOnDevice();
    kernel->input( inwrapper );
    kernel->output( outwrapper );
    int globalSize = N;
    int workgroupsize = cl->getMaxWorkgroupSize();
    globalSize = ( ( globalSize + workgroupsize - 1 ) / workgroupsize ) * workgroupsize;
    cout << "globalsize: " << globalSize << " workgroupsize " << workgroupsize << endl;
    kernel->run_1d( globalSize, workgroupsize );
    outwrapper->copyToHost();
    for( int i = 0; i < N; i++ ) {
       if( out[i] != 689514 ) {
           cout << "out[" << i << "] != 689514: " << out[i] << endl;
           exit(-1);
       }
    }

    delete outwrapper;
    delete inwrapper;
    delete kernel;
    delete cl;
}

