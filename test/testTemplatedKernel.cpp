// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "test/easycl_gtest_supp.h"

#include "EasyCL.h"
#include "templates/TemplatedKernel.h"

using namespace std;
using namespace easycl;

TEST(testTemplatedKernel, basic) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(int N, global {{type}} *out, global const {{type}} *in) {\n"
        "   int globalId = get_global_id(0);\n"
        "   if(globalId < N) {\n"
        "       {{type}} value = in[globalId];\n"
        "       out[globalId] = value;\n"
        "   }\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
    kernelBuilder.set("type", "int");
    CLKernel *kernel = kernelBuilder.buildKernel("doStuff_int", "testfile", kernelSource, "doStuff");
//    kernel = kernela.getKernel();
    int a[2];
    int b[2];
    b[0] = 3;
    b[1] = 2;
    kernel->in(2)->out(2, a)->in(2, b)->run_1d(16, 16);
    cl->finish();
    EXPECT_EQ(3, a[0]);
    EXPECT_EQ(2, a[1]);

//    TemplatedKernel kernelb(cl, "testfile", kernelSource, "doStuff");
    kernelBuilder.set("type", "float");
    kernel = kernelBuilder.buildKernel("doStuff_float", "testfile", kernelSource, "doStuff");
    float ac[2];
    float bc[2];
    bc[0] = 3.2f;
    bc[1] = 2.5f;
    kernel->in(2)->out(2, ac)->in(2, bc)->run_1d(16, 16);
    cl->finish();
    EXPECT_EQ(3.2f, ac[0]);
    EXPECT_EQ(2.5f, ac[1]);

    delete cl;
}

TEST(testTemplatedKernel, withbuilderror) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(int N, global {{type}} *out, global const {{type}} *in) {\n"
        "   int globalId = get_global_id(0);\n"
        "   if(globalId < N) {\n"
        "       {{type}} value = in[globalId];\n"
        "       out[globalId] = value;\n"
        "   }\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
    kernelBuilder.set("type", "int");
    try {
        kernelBuilder.buildKernel("doStuff_int", "testfile", kernelSource, "doStuffaaa");
    } catch(runtime_error &e) {
        cout << "caught error: " << e.what() << endl;
    }

    delete cl;
}

TEST(testTemplatedKernel, withtemplateerror) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(int N, global {{type}} *out, global const {{type}} *in) {\n"
        "   int globalId = get_global_id(0);\n"
        "   if(globalId < N) {\n"
        "       {{type}} value = in[globalId];\n"
        "       out[globalId] = value;\n"
        "   }\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
//    kernelBuilder.set("type", "int");
    try {
        kernelBuilder.buildKernel("doStuff_int", "testfile", kernelSource, "doStuff");
    } catch(runtime_error &e) {
        cout << "caught error: " << e.what() << endl;
    }

    delete cl;
}

TEST(testTemplatedKernel, withbuilderrorintargs) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "\n"
        "void op() {}\n"
        "kernel void doStuff(int N, global float *a) {\n"
        "  op(N)\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
    try {
        kernelBuilder.buildKernel("doStuff", "testfile", kernelSource, "doStuff");
    } catch(runtime_error &e) {
        cout << "caught error: " << e.what() << endl;
    }

    delete cl;
}

TEST(testTemplatedKernel, withargserror) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(int N, global {{type}} *out, global const {{type}} *in) {\n"
        "   int globalId = get_global_id(0);\n"
        "   if(globalId < N) {\n"
        "       {{type}} value = in[globalId];\n"
        "       out[globalId] = value;\n"
        "   }\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
    kernelBuilder.set("type", "int");
    CLKernel *kernel = kernelBuilder.buildKernel("doStuff", "testfile", kernelSource, "doStuff");
    try {
        kernel->run_1d(16,16);
    } catch(runtime_error &e) {
        cout << "caught error: " << e.what() << endl;
    }

    delete cl;
}

TEST(testTemplatedKernel, basic2) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(global int *value) {\n"
        "   int globalId = get_global_id(0);\n"
        "   if(globalId == 0) {\n"
        "       value[0] = {{myvalue}};\n"
        "   }\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
    for(int i = 0; i < 10; i++) {
        kernelBuilder.set("myvalue", i);
        CLKernel *kernel = kernelBuilder.buildKernel("doStuff_" + easycl::toString(i), "testfile", kernelSource, "doStuff");
        int a[1];
        kernel->out(1, a)->run_1d(16, 16);
        cl->finish();
        cout << "i=" << i << " a[0]=" << a[0] << endl;
        EXPECT_EQ(i, a[0]);
    }

    delete cl;
}

TEST(testTemplatedKernel, foreach) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(global int *value) {\n"
        "   int globalId = get_global_id(0);\n"
        "   if(globalId == 0) {\n"
        "       value[0] = {{myvalue}};\n"
        "       {% for _,name in ipairs(names) do %}\n"
        "           int {{name}} = 0;\n"
        "       {% end %}\n"
        "   }\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
    vector<string> names;
    names.push_back("blue");
    names.push_back("red");
    names.push_back("green");
    kernelBuilder.set("names", names);
    kernelBuilder.set("myvalue", 3);
    kernelBuilder.buildKernel("doStuff_foo", "testfile", kernelSource, "doStuff");
    kernelBuilder.buildKernel("doStuff_foo", "testfile", kernelSource, "doStuff");
    kernelBuilder.buildKernel("doStuff_foo", "testfile", kernelSource, "doStuff");

    delete cl;
}
TEST(testTemplatedKernel, forrange) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(global int *value) {\n"
        "   int globalId = get_global_id(0);\n"
        "   if(globalId == 0) {\n"
        "       value[0] = {{myvalue}};\n"
        "       {% for i=0,4 do %}\n"
        "           int a{{i}} = 0;\n"
        "       {% end %}\n"
        "   }\n"
        "}\n";
    TemplatedKernel kernelBuilder(cl);
    kernelBuilder.set("myvalue", 3);
    kernelBuilder.buildKernel("doStuff_3", "testfile", kernelSource, "doStuff");
    kernelBuilder.buildKernel("doStuff_3", "testfile", kernelSource, "doStuff");
    kernelBuilder.buildKernel("doStuff_3", "testfile", kernelSource, "doStuff");

    delete cl;
}
TEST(testTemplatedKernel, forrange2) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();

    string kernelSource = "kernel void doStuff(\n"
        "{% for t=1,num_tensors do %}\n"
         "   {% local thisdim = loadstring('return dims' .. t)() %}\n"
         "  {% for i=1,thisdim do %}\n"
         "    int t{{t}}{{i}},\n"
         "  {% end %}\n"
        "{% end %}\n"
        " int count\n"
        ") {\n"
        "}\n"
        ;
    TemplatedKernel kernelBuilder(cl);
    kernelBuilder.set("num_tensors", 3);
    kernelBuilder.set("dims1", 2);
    kernelBuilder.set("dims2", 3);
    kernelBuilder.set("dims3", -2);
    cout << kernelBuilder.getRenderedKernel(kernelSource) << endl;
    kernelBuilder.buildKernel("doStuff_3", "testfile", kernelSource, "doStuff");

    delete cl;
}

