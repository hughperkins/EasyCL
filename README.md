OpenCLHelper
============

Easy to run kernels using OpenCL

Example Usage
-------------

Imagine we have a kernel with the following signature, in the file /tmp/foo.cl:

    kernel void my_kernel( int N, global float *one, global float *two, local float *one_local, global float *result ) {
        // kernel code here...
    }

... then we can call it like:

    #include "OpenCLHelper.h"

    void run_kernel( int N, const float *one, const float *two, float *result ) {
        OpenCLHelper cl(0, "/tmp/foo.cl", "my_kernel");
        cl.input( N );
        cl.input( N, one );
        cl.input( N, two );
        cl.local( N );
        cl.output( N, result );
        const size_t local_ws[1]; local_ws[0] = 512;
        const size_t global_ws[1]; global_ws[0] = OpenCLHelper::roundUp(local_ws[0], size);
        cl.run( 1, global_ws, local_ws );
        // result now contains the result of running the kernel, and all resources used
        // have been cleaned up
    }

API
---

    // constructor:
    OpenCLHelper::OpenCLHelper( int GPUIndex, string kernelfilepath, string kernelname );

    // passing arguments to kernel:
    OpenCLHelper::input( int integerinput );
    OpenCLHelper::input( int arraysize, const float *inputarray ); // size in number of floats
    OpenCLHelper::output( int arraysize, float *outputarray ); // size in number of floats
    OpenCLHelper::local( int localarraysize ); // size in number of floats

    // running kernel, getting result back, and cleaning up:
    OpenCLHelper::run( int number_dimensions, size_t *global_ws, size_t *local_ws );

    // helper function:
    OpenCLHelper::roundUp( int quantizationSize, int desiredTotalSize );

License
-------

OpenCLHelper is available under MPL v2 license, http://mozilla.org/MPL/2.0/

