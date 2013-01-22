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

    void run_kernel( int N, const float *one, const float *two, float *result ) {
        if( !OpenCLHelper::isOpenCLAvailable() ) {
            cout << "opencl library not found" << endl;
            exit(-1);
        }
        cout << "found opencl library" << endl;
        OpenCLHelper cl(0);
        CLKernel *kernel = cl.buildKernel( "/tmp/foo.cl", "my_kernel");
        kernel->input( N );
        kernel->input( N, one );
        kernel->input( N, two );
        kernel->local( N );
        kernel->output( N, result );
        const size_t local_ws[1]; local_ws[0] = 512;
        const size_t global_ws[1]; global_ws[0] = OpenCLHelper::roundUp(local_ws[0], size);
        kernel->run( 1, global_ws, local_ws );
        // result now contains the result of running the kernel
        delete kernel; // cleanup
    }

There is a simple example in the 'test' subdirectory.

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
    CLKernel::run( int number_dimensions, size_t *global_ws, size_t *local_ws );

    // helper function:
    OpenCLHelper::roundUp( int quantizationSize, int desiredTotalSize );

CLArray1d objects
-----------------

To make it possible to reuse data between kernels, without moving back to PC
main memory, and back onto the GPU, you can use CLArray1d objects.

These can be created on the GPU, or on the host, and moved backwards 
and forwards between each other, as required.  They can be passed as an 'input'
and 'output' to a CLKernel object.  They can be reused between kernels.

    OpenCLHelper cl(0);

    CLArray1d *one = cl.array1d(10000); // create CLArray1d object for 10,000 floats
    one->createOnHost(); // allocate on the host
    (*one)[0] = 5;
    (*one)[1] = 7; // give some data...

    // create on device:
    CLArray1d *two = cl.array1d(10000);
    two->createOnDevice();

    // pass to kernel:
    kernel->input(one);
    kernel->output(two);

You can then take the 'two' CLArray1d object, and pass it as the 'input' to 
a different kernel, or you can use operator[] to read values from it.

License
-------

OpenCLHelper is available under MPL v2 license, http://mozilla.org/MPL/2.0/

