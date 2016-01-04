<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [EasyCL ](#easycl)
  - [Example Usage](#example-usage)
  - [Examples](#examples)
  - [API](#api)
  - [CLArray and CLWrapper objects](#clarray-and-clwrapper-objects)
  - [CLWrapper objects](#clwrapper-objects)
  - [CLArray objects](#clarray-objects)
- [Kernel store](#kernel-store)
- [device dirty flag](#device-dirty-flag)
- [kernel run-time templates](#kernel-run-time-templates)
- [templated kernels (New!)](#templated-kernels-new)
- [passing structs (New!)](#passing-structs-new)
  - [How to build](#how-to-build)
    - [Building on linux](#building-on-linux)
      - [Pre-requisites](#pre-requisites)
      - [Optional requirements](#optional-requirements)
      - [Procedure](#procedure)
    - [Building on Windows](#building-on-windows)
      - [Pre-requisites](#pre-requisites-1)
      - [Optional requirements](#optional-requirements-1)
      - [Procedure](#procedure-1)
  - [How to run self-tests](#how-to-run-self-tests)
  - [How to check my OpenCL installation/configuration?](#how-to-check-my-opencl-installationconfiguration)
  - [What if I've found a bug?](#what-if-ive-found-a-bug)
  - [What if I want a new feature?](#what-if-i-want-a-new-feature)
  - [What if I just have a question?](#what-if-i-just-have-a-question)
- [Recent changes](#recent-changes)
  - [License](#license)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

EasyCL 
============

Easy to run kernels using OpenCL.  (renamed from OpenCLHelper)

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

    #include "EasyCL.h"

    if( !EasyCL::isOpenCLAvailable() ) {
        cout << "opencl library not found" << endl;
        exit(-1);
    }
    EasyCL *cl = EasyCL::createForFirstGpu();
    CLKernel *kernel = cl->buildKernel("somekernelfile.cl", "test_function");
    int in[5];
    int out[5];
    for( int i = 0; i < 5; i++ ) {
        in[i] = i * 3;
    }
    kernel->in( 5, in );
    kernel->out( 5, out );
    kernel->run_1d( 5, 5 ); // global workgroup size = 5, local workgroup size = 5
    delete kernel;
    // use the results in 'out' array here

More generally, you can call on 2d and 3d workgroups by using the `kernel->run` method:

    const size_t local_ws[1]; local_ws[0] = 512;
    const size_t global_ws[1]; global_ws[0] = EasyCL::roundUp(local_ws[0], size);
    kernel->run( 1, global_ws, local_ws ); // 1 is number of dimensions, could be 2, or 3

'Fluent' style is also possible, eg:

    kernel->in(10)->in(5)->out( 5, outarray )->run_1d( 5, 5 );

If you use `EasyCL::createForFirstGpu()`, EasyCL will bind to the first OpenCL-enabled GPU (or accelerator), that it finds.  If you want to use a different device, or an OpenCL-enabled CPU, you can use one of the following method:
```c++
EasyCL::createForIndexedGpu( int gpuindex ); // looks for opencl-enabled gpus, and binds to the (gpuindex+1)th one
EasyCL::createForFirstGpuOtherwiseCpu();
EasyCL::createForPlatformDeviceIndexes( int platformIndex, int deviceIndex );
EasyCL::createForPlatformDeviceIds( int platformId, int deviceId ); // you can get these ids by running `gpuinfo` first
```

You can run `gpuinfo` to get a list of platforms and devices on your system.

There are some examples in the [test](test) subdirectory.

Examples
--------

There are some examples in the [test](test) subdirectory.
- create a couple of Wrapper objects, pass them into a kernel, look at the results, see method ` testfloatwrapper, main )` of [testfloatwrapper.cpp](test/testfloatwrapper.cpp)
- (New!) to use with clBLAS, see [testclblas.cpp](test/testclblas.cpp)

API
---

    // constructor:
    EasyCL::EasyCL();
    // choose different gpu index
    void EasyCL::gpu( int gpuindex );

    // compile kernel
    CLKernel *EasyCL::buildKernel( string kernelfilepath, string kernelname, string options = "" );

    // Note that you pass `#define`s in through the `options` parameters, like `-D TANH`, or `-D TANH -D BIASED`

    // passing arguments to kernel:

    CLKernel::in( int integerinput );

    CLKernel::in( int arraysize, const float *inputarray ); // size in number of floats
    CLKernel::in( int arraysize, const int *inputarray ); // size in number of ints
    CLKernel::out( int arraysize, float *outputarray ); // size in number of floats
    CLKernel::out( int arraysize, int *outputarray ); // size in number of ints
    CLKernel::inout( int arraysize, float *inoutarray ); // size in number of floats
    CLKernel::inout( int arraysize, int *inoutarray ); // size in number of ints

    // to allocate local arrays, as passed-in kernel parameters:
    CLKernel::localFloats( int localarraysize ); // size in number of floats
    CLKernel::localInts( int localarraysize ); // size in number of ints

    // running kernel, getting result back, and cleaning up:
    CLKernel::run_1d( int global_ws, int local_ws );
    CLKernel::run( int number_dimensions, size_t *global_ws, size_t *local_ws );

    // helper function:
    EasyCL::roundUp( int quantizationSize, int desiredTotalSize );

CLArray and CLWrapper objects
-----------------------------

To make it possible to reuse data between kernels, without moving back to PC
main memory, and back onto the GPU, you can use CLWrapper objects.

These can be created on the GPU, or on the host, and moved backwards 
and forwards between each other, as required.  They can be passed as an 'input'
and 'output' to a CLKernel object.  They can be reused between kernels.

There are two 'flavors':
- CLArray: more automated, but more memory copying, since creates a new array 
on the host
- CLWrapper: wraps an existing host array, you'll need to call `copyToDevice()` and
`copyToHost()` yourself

CLArray objects are the first implementation.  CLWrapper objects are the second implementation.
You can use either, but note that CLWrapper objects are the ones that I use myself.

CLWrapper objects
-----------------

Compared to CLArray objects, CLWrapper objects need less memory copying,
since they wrap an existing native array, but you will need to call `copyToDevice()`
and `copyToHost()` yourself.

```c++
if( !EasyCL::isOpenCLAvailable() ) {
    cout << "opencl library not found" << endl;
    exit(-1);
}
cout << "found opencl library" << endl;

EasyCL cl;
CLKernel *kernel = cl.buildKernel("../test/testeasycl.cl", "test_int");
int in[5];
for( int i = 0; i < 5; i++ ) {
    in[i] = i * 3;
}
int out[5];
CLWrapper *inwrapper = cl.wrap(5, in);
CLWrapper *outwrapper = cl.wrap(5, out);
inwrapper->copyToDevice();
kernel->in( inwrapper );
kernel->out( outwrapper );
kernel->run_1d( 5, 5 );
outwrapper->copyToHost();
assertEquals( out[0] , 7 );
assertEquals( out[1] , 10 );
assertEquals( out[2] , 13 );
assertEquals( out[3] , 16 );
assertEquals( out[4] , 19 );
cout << "tests completed ok" << endl;
```

Can copy between buffers (New!):
```c++
wrapper1->copyTo( wrapper2 );
```

CLWrapper objects are currently available as `CLIntWrapper` and `CLFloatWrapper`.

CLArray objects
---------------

Compared to CLWrapper objects, CLArray objects are more automated, but involve more 
memory copying.

```c++
EasyCL cl;

CLArrayFloat *one = cl.arrayFloat(10000); // create CLArray object for 10,000 floats
(*one)[0] = 5; // give some data...
(*one)[1] = 7; 

CLArrayFloat *two = cl.arrayFloat(10000);

// pass to kernel:
kernel->in(one)->out(two);
```

You can then take the 'two' CLArray object, and pass it as the 'input' to 
a different kernel, or you can use operator[] to read values from it.

Currently, CLArray is available as 'CLArrayFloat' and 'CLArrayInt'.

# Kernel store

You can store kernels in the store, under a unique name each, to facilitate kernel caching
```c++
// store:
cl->storeKernel( "mykernelname", somekernel ); // name must be not used yet

// check exists:
cl->kernelExists( "mykernelname" );

// retrieve:
CLKernel *kernel = cl->getKernel( "mykernelname" );
```

New: you can transfer kernel ownership to EasyCL object, by passing third parameter `deleteWithCl = true`.  Then, when the EasyCL object is deleted, so will be the kernel.
```c++
// store:
cl->storeKernel( "mykernelname", somekernel, true ); // this kernel will be deleted when
                                                     // `cl` object is deleted
```

# device dirty flag

For CLWrapper objects, if the wrapper is passed to a kernel via `out` or `inout`, and then that kernel is run, then `isDeviceDirty()` will return true, until `->copyToHost()` is called.  So, you can use this to determine whether you need to run `->copyToHost()` prior to reading the host-side array.

The following methods will reset the flag to `false`:
* `copyToDevice()`
* `copyToHost()`

This is a new feature, as of May 15 2015, and might have some bugs prior to May 31 2015 (ie, about 2 weeks, long enough for me to find any bugs).

# templated kernels

* You can use templating with kernels, at runtime, using the build-in Lua engine
* Simple variable substitution by using eg `{{some_variable_name}}`
* Embed lua code, including loops, if statements etc, ... using eg `{% for i=0,5 do %}... code here ... {% end %}`
* The magic is done using the [templates/TemplatedKernel.h](templates/TemplatedKernel.h] class
* See examples in [test/testTemplatedKernel.cpp](test/testTemplatedKernel.cpp)
* Note that this templating method is based on John Nachtimwald's work at [https://john.nachtimwald.com/2014/08/06/using-lua-as-a-templating-engine/](https://john.nachtimwald.com/2014/08/06/using-lua-as-a-templating-engine/) ( [MIT License](https://john.nachtimwald.com/files/2008/11/MIT.txt) )

# passing structs

* Simply `#include` new `"CLKernel_structs.h"` header, in order to be able to pass structs
* See [test/testStructs.cpp](test/testStructs.cpp) for an example

# Profiling (New!)

* Simply call `cl->setProfiling(true);`, then run your kernels as normal, then call `cl->dumpProfiling` to print the results
* Timings are cumulative over multiple calls to the same kernel
* Timings are grouped by kernel filename and kernelname
* See [test/testprofiling.cpp](test/testprofiling.cpp) for an example

# Using with clBLAS

* You can call `->getBuffer()` on a CLWrapper object, in order to pass it to clBLAS.  You can see an example eg at [THClBlas.cpp#L425](https://github.com/hughperkins/cltorch/blob/b6a226722a6ee7bd55b4729e5bf12c7c700d3da3/lib/THCl/THClBlas.cpp#L425)

## How to build

### Build options

|Option|Description|
|-------|---------|
|`PROVIDE_LUA_ENGINE| If you want to call EasyCL from within Lua, then choose option `PROVIDE_LUA_ENGINE=OFF`, otherwise leave it as `ON` |
| `DEV_RUN_COG` | Only for EasyCL maintainers, leave as `OFF` otherwise |
| `BUILD_TESTS` | whether to build unit tests|

[![Build Status](https://travis-ci.org/hughperkins/EasyCL.svg?branch=master)](https://travis-ci.org/hughperkins/EasyCL)

### Building on Mac OS X

(tested on Travis https://travis-ci.org/hughperkins/EasyCL )

#### Pre-requisites

- git
- cmake
- g++
- (maybe) OpenCL (not sure if installed by default?  Travis worked ok without explicitly installing it)

#### Procedure

```bash
git clone --recursive https://github.com/hughperkins/EasyCL.git
cd EasyCL
mkdir build
cd build
cmake ..
make install
```
* the executables will be in the `../dist/bin` folder, and the .dylib files in `../dist/lib`
* Dont forget the `--recursive`, otherwise you will see odd errors about `clew/src/clew.c` missing
  * If this happens, you can try `git submodule init` and then `git submodule update`.

### Building on linux

#### Pre-requisites

- OpenCL needs to be installed, which means things like:
  - in linux, you'll need a libOpenCL.so installed, and 
  - an OpenCL implementation, ie some kind of .so file, and
  - an appropriate text file at /etc/OpenCL/vendors/somename.icd , containing the full path to the Open
CL implementation .so file
- git  (only needed to obtain the source-code)
- cmake
- g++

#### Procedure

```bash
git clone --recursive https://github.com/hughperkins/EasyCL.git
cd EasyCL
mkdir build
cd build
cmake ..
make install
```
* the executables will be in the `../dist/bin` folder, and the .so files in `../dist/lib`
* Dont forget the `--recursive`, otherwise you will see odd errors about `clew/src/clew.c` missing
  * If this happens, you can try `git submodule init` and then `git submodule update`.

### Building on Windows

#### Pre-requisites

- OpenCL-enabled GPU and driver
- git  (only needed to obtain the source-code)
- cmake
- Visual Studio (tested with Visual Studio 2013 Community Edition)

#### Procedure

* Open git bash, and run `git clone --recursive https://github.com/hughperkins/EasyCL.git`
* Open cmake:
  * set source directory to the git-cloned directory from previous step
  * Set build directory to a subdirectory `build-win32`, or `build-win64`, according to which platform you are building for
  * click `configure`, choose appropriate build platform, eg visual studio 2013, or visual studio 2013 win64
  * click `generate`
* Open visual studio
  * open any of the projects in the `build-win32` or `build-win64` build directory
  * change build type from `Debug` to `Release`
  * from `build` menu, choose `build solution`
  * right-click 'INSTALL' project, and select 'Build'
* after building, you will need to copy the *.cl files from the `test` directory into the directory where you will run the tests from (if you can figure out a way to automate this, please send a pull request :-) )

How to run self-tests
---------------------

To check clew library is working ok (ie finding and loading the opencl library, etc):

linux:
```
    LD_LIBRARY_PATH=../dist/lib ..dist/bin/gpuinfo
```
Windows:
```
    ..dist/bin/gpuinfo
```

... should print some information about your graphics card

*Unit-tests:*

Linux:
```
    LD_LIBRARY_PATH=../dist/lib ..dist/bin/easycl_unittests
```
Windows:
```
    ..dist/bin/easycl_unittests
```

How to check my OpenCL installation/configuration?
--------------------------------------------------

- In Ubuntu, you can use `clinfo` (install via `sudo apt-get install clinfo`), to check the
OpenCL installation itself is ok.  If this says 'no installations found', then it's an OpenCL
configuration issue.
  - note that `clinfo` is broken on CUDA, I think?  But OpenCL will still work ok: try `gpuinfo` instead
- Run `gpuinfo` to list available platforms and devices
- If no gpu-capabable devices found, you probably want to check things like:
  - do you have an OpenCL-capable GPU installed?
  - are the drivers installed?
  - is the ICD setup?

What if I've found a bug?
----------------

* Ideally, create a *simple* test case, just 10-30 lines if possible, and either just paste it directly as an [issue](https://github.com/hughperkins/EasyCL/issues), or else fork the repository, and ideally add it into the [test](https://github.com/hughperkins/EasyCL/tree/master/test) directory, as an additional gtest-compliant test.
* (and then, obviously post an [issue](https://github.com/hughperkins/EasyCL/issues) to alert me)

What if I want a new feature?
-----------------------------

* Post a request as an [issue](https://github.com/hughperkins/EasyCL/issues)
* Or, fork the repository, add the feature, and send me a [pull request](https://github.com/hughperkins/EasyCL/pulls)

What if I just have a question?
-------------------------------

* Post as an [issue](https://github.com/hughperkins/EasyCL/issues)

# Recent changes

* 2016 Jan 3rd:
  * create Mac OS X build on Travis https://travis-ci.org/hughperkins/EasyCL , which passes
* 2015 Sep 10th:
  * fix mac build
  * merge to master
* 2015 Aug 28th:
  * add USE_CLEW option, default 'ON', but can disable, to link directly with OpenCL libraries, rather than via clew
* 2015 Aug 26th:
  * int64 and uint64 are now typedef'd to `int64_t` and `uint64_t`, instead of `long long` and `unsigned long long`.  This is configurable in cmake options, though the default is that the typedef changes.  I'm not 100% sure if changing the default is a good idea, but it seems better than having `int64` and `int64_t` be two different types...
* 2015 Aug 15th:
  * builds again on Windows (as well as on Ubuntu 14.04)
* 2015 Aug 8th:
  * merged development branches into master.  changes include:
    * clew is now a git submodule again.  Make sure to do `git submodule init` and `git submodulate update` to download it
      * when you checkout / update, you might need to use `-f` option to git, or delete the thirdparty/clew directory first
    * added build option to not link with internal Lua library
    * added profiling, using the OpenCL profiling functions
    * copy of data between host and device is done using explicit enqueueCopyBuffer functions now
* 2015 July 15th:
  * added profiling
* 2015 June 27th:
  * merged bundle-lua to master
  * Added StatefulTimer.h (was in [DeepCL](http://github.com/hughperkins/DeepCL) )
* 2015 June 18-25th:
  * on branch bundle-lua:
    * builds on Windows again
    * started bundling lua sourcecode, so dont need lua libraries etc
    * Added new version of CLWrapper->copyTo, wihch has additional parameters `, srcOffset, dstOffset, count`
    * Added StatefulTimer.h (was in [DeepCL](http://github.com/hughperkins/DeepCL)
* 2015 June 17th:
  * Merged changes to master:
    * DevicesInfo::getNumDevices() now returns 0, if no platforms available, rather than throwing exception
    * templates are expanded recursively now, so eg you can include other templates inside your template, and those will be expanded correctly (if you dislike this, please raise an issue to make it an option; easy to add)
    * added EasyCL::createForIndexedDevice, which creates an instance for the indexed device, over all opencl-enabled devices, gpu or not
    * more diagnostic output if a kernel fails to build, including line-numbers :-)
    * added getRenderedKernel method to KernelTemplater class
* 2015 June:
  * added kernel templates, using Lua
  * added `CLWrapper->copyTo()` method
  * made it possible to pass arrays of 1 or more structs into `CLKernel`s
  * added install targets to the build
  * added options to the build to turn unit-tests on/off
* 2015 May 11: just noticed there is an over-aggressive assert in gpuinfo, that exits if not exactly one platform => fixed
* 2015 May 10: Added CLWrapper.devicedirty flag, which is set whenever the wrapper is passed to a kernel via `out` or `inout`, and that kernel is run
* 2015 May 3: Added kernel store methods: `storeKernel( string name, CLKernel *kernel )`, `getKernel( string name )`, `kernelExists( string name )`, to facilitate per-connection kernel caching
* 2015 May 3: Added `getCl()` to `CLWrapper` types
* 2015 May 1:Renamed from OpenCLHelper to EasyCL (easier to type, and remember)
* Added getBuffer to CLWrapper, to give access to the underlying buffer, eg can use this for using with clBLAS
* Added CLWrapper instantiation for unsigned char

License
-------

EasyCL is available under MPL v2 license, http://mozilla.org/MPL/2.0/

