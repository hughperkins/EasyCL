OpenCLHelper
============

Easy to run kernels using OpenCL.

- makes it easy to pass input and output arguments
- handles much of the boilerplate
- uses clew to load opencl dynamically

Example Usage
-------------

Imagine we have a kernel with the following signature, in the file /tmp/foo.cl:

    kernel void my_kernel( int N, global float *one, global float *two, local float *one_local, global float *result ) {
        // kernel code here...
    }

... then we can call it like:

    #include "OpenCLHelper.h"

    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    OpenCLHelper cl(0);
    CLKernel *kernel = cl.buildKernel("somekernelfile.cl", "test_function");
    int in[5];
    int out[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    kernel->input( 5, in );
    kernel->output( 5, out );
    kernel->run_1d( 5, 5 ); // global workgroup size = 5, local workgroup size = 5
    delete kernel;
    // use the results in 'out' array here

More generally, you can call on 2d and 3d workgroups by using the `kernel->run` method:

    const size_t local_ws[1]; local_ws[0] = 512;
    const size_t global_ws[1]; global_ws[0] = OpenCLHelper::roundUp(local_ws[0], size);
    kernel->run( 1, global_ws, local_ws ); // 1 is number of dimensions, could be 2, or 3

There are some examples in the 'test' subdirectory.

API
---

    // constructor:
    OpenCLHelper::OpenCLHelper( int GPUIndex );

    // compile kernel
    CLKernel *OpenCLHelper::buildKernel( string kernelfilepath, string kernelname );

    // passing arguments to kernel:

    CLKernel::input( int integerinput );

    CLKernel::input( int arraysize, const float *inputarray ); // size in number of floats
    CLKernel::input( int arraysize, const int *inputarray ); // size in number of ints
    CLKernel::output( int arraysize, float *outputarray ); // size in number of floats
    CLKernel::output( int arraysize, int *outputarray ); // size in number of ints
    CLKernel::inout( int arraysize, float *inoutarray ); // size in number of floats
    CLKernel::inout( int arraysize, int *inoutarray ); // size in number of ints

    CLKernel::local( int localarraysize ); // size in number of floats

    // running kernel, getting result back, and cleaning up:
    CLKernel::run_1d( int global_ws, int local_ws );
    CLKernel::run( int number_dimensions, size_t *global_ws, size_t *local_ws );

    // helper function:
    OpenCLHelper::roundUp( int quantizationSize, int desiredTotalSize );

CLArray objects
---------------

To make it possible to reuse data between kernels, without moving back to PC
main memory, and back onto the GPU, you can use CLArray objects.

These can be created on the GPU, or on the host, and moved backwards 
and forwards between each other, as required.  They can be passed as an 'input'
and 'output' to a CLKernel object.  They can be reused between kernels.

Compared to CLWrapper objects, they are more automated, but involve more 
memory copying.  They're more mature.

    OpenCLHelper cl(0);

    CLArrayFloat *one = cl.arrayFloat(10000); // create CLArray object for 10,000 floats
    one->createOnHost(); // allocate on the host
    (*one)[0] = 5;
    (*one)[1] = 7; // give some data...

    // create on device:
    CLArrayFloat *two = cl.arrayFloat(10000);
    two->createOnDevice();

    // pass to kernel:
    kernel->input(one);
    kernel->output(two);

You can then take the 'two' CLArray object, and pass it as the 'input' to 
a different kernel, or you can use operator[] to read values from it.

Currently, CLArray is available as 'CLArrayFloat' and 'CLArrayInt'.

CLWrapper objects
-----------------

To make it possible to reuse data between kernels, without moving back to PC
main memory, and back onto the GPU, you can use CLArray objects.

These can be created on the GPU, or on the host, and moved backwards 
and forwards between each other, as required.  They can be passed as an 'input'
and 'output' to a CLKernel object.  They can be reused between kernels.

Compared to CLArray objects, CLWrapper objects are currently in 'draft' status,
but involve less memory copying.  They're less automated, since you 
have to explicitly call 'copyToHost()' and 'copyToDevice()'.

    if( !OpenCLHelper::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    cout << "found opencl library" << endl;

    OpenCLHelper cl(0);
    CLKernel *kernel = cl.buildKernel("../test/testopenclhelper.cl", "test_int");
    int in[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    int out[5];
    CLIntWrapper *inwrapper = cl.intWrapper(5, in);
    CLIntWrapper *outwrapper = cl.intWrapper(5, out);
    inwrapper->copyToDevice();
    kernel->input( inwrapper );
    kernel->output( outwrapper );
    kernel->run_1d( 5, 5 );
    outwrapper->copyToHost();
    assertEquals( out[0] , 7 );
    assertEquals( out[1] , 10 );
    assertEquals( out[2] , 13 );
    assertEquals( out[3] , 16 );
    assertEquals( out[4] , 19 );
    cout << "tests completed ok" << endl;

Pre-requisites
--------------

- OpenCL needs to be installed, which means things like:
  - in linux, you'll need a libOpenCL.so installed, and 
  - an OpenCL implementation, ie some kind of .so file, and
  - an appropriate text file at /etc/OpenCL/vendors/somename.icd , containing the full path to the Open
CL implementation .so file
- CMake
- C++ compiler
- git

How to build
------------

    git clone https://github.com/hughperkins/OpenCLHelper.git
    cd OpenCLHelper
    mkdir build
    cd build
    cmake ..
    make

How to run self-tests
---------------------

    ./gpuinfo

... should print some information about your graphics card

    ./testopenclhelper

... will compile a kernel, pass in some data, run the kernel, and check the results
are expected

What if it doesn't run?
-----------------------

- In Ubuntu, you can use `clinfo` (install via `sudo apt-get install clinfo`), to check the
OpenCL installation itself is ok.  If this says 'no installations found', then it's an OpenCL
configuration issue.

What if it runs, but no results?
--------------------------------

- check types, eg if you pass in int arrays, but the kernel expects floats, results might not
  be what you were hoping to see

License
-------

OpenCLHelper is available under MPL v2 license, http://mozilla.org/MPL/2.0/

