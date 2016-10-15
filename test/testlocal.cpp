#include "EasyCL.h"
#include <iostream>
using namespace std;

#include "gtest/gtest.h"

// #include "Timer.h"

static const char *getKernel();

using namespace easycl;

TEST(testlocal, uselocal) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "useLocal", "");
    int workgroupSize = 64;
    float *myarray = new float[workgroupSize];

    kernel->in(workgroupSize);
    kernel->inout(workgroupSize, myarray);
    kernel->localFloats(workgroupSize);
    kernel->run_1d(workgroupSize, workgroupSize);

    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(testlocal, notUselocal) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "notUseLocal", "");
    int workgroupSize = 64;
    float *myarray = new float[workgroupSize];

    kernel->in(workgroupSize);
    kernel->inout(workgroupSize, myarray);
    kernel->run_1d(workgroupSize, workgroupSize);

    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(testlocal, globalreduce) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "reduceGlobal", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    float *myarray = new float[workgroupSize];
//    Timer timer;
    for(int i = 0; i < 2000; i++) {
        float sumViaCpu = 0;
        for(int i = 0; i < workgroupSize; i++) {
            myarray[i] = (i + 7) * 3;
            sumViaCpu += myarray[i];
        }
        EXPECT_NE(myarray[0], sumViaCpu);

        kernel->inout(workgroupSize, myarray);
        kernel->run_1d(workgroupSize, workgroupSize);
        
        EXPECT_EQ(myarray[0], sumViaCpu);
    }
//    timer.timeCheck("after iterations");
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(testlocal, localreduce) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "reduceViaScratch", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    float *myarray = new float[workgroupSize];
//    Timer timer;
    for(int i = 0; i < 2000; i++) {
        float sumViaCpu = 0;
        for(int i = 0; i < workgroupSize; i++) {
            myarray[i] = (i + 7) * 3;
            sumViaCpu += myarray[i];
        }
        EXPECT_NE(myarray[0], sumViaCpu);

        kernel->inout(workgroupSize, myarray)->localFloats(workgroupSize);
        kernel->run_1d(workgroupSize, workgroupSize);
        
        EXPECT_EQ(myarray[0], sumViaCpu);
    }
//    timer.timeCheck("after iterations");
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(testlocal, reduceviascratch_multipleworkgroups) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "reduceViaScratch_multipleworkgroups", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    const int numWorkgroups = workgroupSize;
    const int N = workgroupSize * numWorkgroups;
    float *myarray = new float[N];
    float sumViaCpu = 0;
    float localSumViaCpu = 0;
    for(int i = 0; i < N; i++) {
        myarray[i] = ((i + 7) * 3) % 10;
        sumViaCpu += myarray[i];
        if(i < workgroupSize) {
            localSumViaCpu += myarray[i];
        }
    }
    cout << "expected sum, calc'd via cpu, : " << sumViaCpu << endl;
    EXPECT_NE(myarray[0], sumViaCpu);

//    Timer timer;

    CLWrapper *a1wrapper = cl->wrap(N, myarray);
    a1wrapper->copyToDevice();
    float *a2 = new float[numWorkgroups];
    CLWrapper *a2wrapper = cl->wrap(numWorkgroups, a2);
    kernel->in(a1wrapper);
    kernel->out(a2wrapper);
    kernel->localFloats(workgroupSize);
    kernel->run_1d(N, workgroupSize);

    float finalSum;
    kernel->in(a2wrapper);
    kernel->out(1, &finalSum);
    kernel->localFloats(workgroupSize);
    kernel->run_1d(numWorkgroups, workgroupSize);

    EXPECT_EQ(sumViaCpu, finalSum);

    delete a1wrapper;
    delete a2wrapper;
    delete[] a2;
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(testlocal, reduceviascratch_multipleworkgroups_ints) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "reduceViaScratch_multipleworkgroups_ints", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    const int numWorkgroups = workgroupSize;
    const int N = workgroupSize * numWorkgroups;
    cout << "numworkgroups " << numWorkgroups << " workgroupsize " << workgroupSize << " N " << N << endl;
    int *myarray = new int[N];
    int sumViaCpu = 0;
    int localSumViaCpu = 0;
    int localSumViaCpu2 = 0;
    int *localSumsViaCpu = new int[numWorkgroups];
    memset(localSumsViaCpu, 0, sizeof(int)*numWorkgroups);
    for(int i = 0; i < N; i++) {
        myarray[i] = ((i + 7) * 3) % 50;
        sumViaCpu += myarray[i];
        if(i < workgroupSize) {
            localSumViaCpu += myarray[i];
        }
        if(i >= workgroupSize && i < workgroupSize * 2) {
            localSumViaCpu2 += myarray[i];
        }
        int workgroupId = i / workgroupSize;
        localSumsViaCpu[workgroupId] += myarray[i];
    }
    ASSERT_EQ(localSumViaCpu, localSumsViaCpu[0]);
    ASSERT_EQ(localSumViaCpu2, localSumsViaCpu[1]);
    ASSERT_NE(myarray[0], sumViaCpu);

//    Timer timer;

    CLWrapper *a1wrapper = cl->wrap(N, myarray);
    a1wrapper->copyToDevice();
    int *a2 = new int[numWorkgroups];
    CLWrapper *a2wrapper = cl->wrap(numWorkgroups, a2);
    kernel->in(a1wrapper);
    kernel->out(a2wrapper);
    kernel->localInts(workgroupSize);
    kernel->run_1d(N, workgroupSize);

    int finalSum;
    kernel->in(a2wrapper);
    kernel->out(1, &finalSum);
    kernel->localInts(workgroupSize);
    kernel->run_1d(numWorkgroups, workgroupSize);
//    timer.timeCheck("finished 2-way reduce");

    EXPECT_EQ(sumViaCpu, finalSum);

    delete a1wrapper;
    delete a2wrapper;
    delete[] a2;
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(testlocal, reduce_multipleworkgroups_ints_noscratch) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "reduce_multipleworkgroups_ints_noscratch", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    const int numWorkgroups = workgroupSize;
    const int N = workgroupSize * numWorkgroups;
    cout << "numworkgroups " << numWorkgroups << " workgroupsize " << workgroupSize << " N " << N << endl;
    int *myarray = new int[N];
    int sumViaCpu = 0;
    int localSumViaCpu = 0;
    int localSumViaCpu2 = 0;
    int *localSumsViaCpu = new int[numWorkgroups];
    memset(localSumsViaCpu, 0, sizeof(int)*numWorkgroups);
    for(int i = 0; i < N; i++) {
        myarray[i] = ((i + 7) * 3) % 50;
        sumViaCpu += myarray[i];
        if(i < workgroupSize) {
            localSumViaCpu += myarray[i];
        }
        if(i >= workgroupSize && i < workgroupSize * 2) {
            localSumViaCpu2 += myarray[i];
        }
        int workgroupId = i / workgroupSize;
        localSumsViaCpu[workgroupId] += myarray[i];
    }
    ASSERT_EQ(localSumViaCpu, localSumsViaCpu[0]);
    ASSERT_EQ(localSumViaCpu2, localSumsViaCpu[1]);
    ASSERT_NE(myarray[0], sumViaCpu);

//    Timer timer;

    CLWrapper *a1wrapper = cl->wrap(N, myarray);
    a1wrapper->copyToDevice();
    int *a2 = new int[numWorkgroups];
    CLWrapper *a2wrapper = cl->wrap(numWorkgroups, a2);
    kernel->in(a1wrapper);
    kernel->out(a2wrapper);
    kernel->run_1d(N, workgroupSize);

    int finalSum;
    kernel->in(a2wrapper);
    kernel->out(1, &finalSum);
    kernel->run_1d(numWorkgroups, workgroupSize);
//    timer.timeCheck("finished 2-way reduce");

    EXPECT_EQ(sumViaCpu, finalSum);

    delete a1wrapper;
    delete a2wrapper;
    delete[] a2;
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(SLOW_testlocal, reduce_noscratch_multipleworkgroups_ints_3levels) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "reduce_multipleworkgroups_ints_noscratch", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    const int numWorkgroups = workgroupSize;
    const int level3size = numWorkgroups / 4;
    const int N = workgroupSize * numWorkgroups * level3size;
    cout << "numworkgroups " << numWorkgroups << " workgroupsize " << workgroupSize << " N " << N << endl;
    int *myarray = new int[N];
    int sumViaCpu = 0;
    int localSumViaCpu = 0;
    int localSumViaCpu2 = 0;
    int *localSumsViaCpu = new int[numWorkgroups * level3size];
    memset(localSumsViaCpu, 0, sizeof(int)*numWorkgroups* level3size);
    for(int i = 0; i < N; i++) {
        myarray[i] = ((i + 7) * 3) % 50;
        sumViaCpu += myarray[i];
        if(i < workgroupSize) {
            localSumViaCpu += myarray[i];
        }
        if(i >= workgroupSize && i < workgroupSize * 2) {
            localSumViaCpu2 += myarray[i];
        }
        int workgroupId = i / workgroupSize;
        localSumsViaCpu[workgroupId] += myarray[i];
    }
    ASSERT_EQ(localSumViaCpu, localSumsViaCpu[0]);
    ASSERT_EQ(localSumViaCpu2, localSumsViaCpu[1]);
    ASSERT_NE(myarray[0], sumViaCpu);

//    Timer timer;

    CLWrapper *a1wrapper = cl->wrap(N, myarray);
    a1wrapper->copyToDevice();
//    timer.timeCheck("copied array to device");
    int *a2 = new int[numWorkgroups*level3size];
    CLWrapper *a2wrapper = cl->wrap(numWorkgroups * level3size, a2);
    kernel->in(a1wrapper);
    kernel->out(a2wrapper);
    kernel->run_1d(N, workgroupSize);

    int *a3 = new int[numWorkgroups];
    CLWrapper *a3wrapper = cl->wrap(level3size, a3);
    kernel->in(a2wrapper);
    kernel->out(a3wrapper);
    kernel->run_1d(workgroupSize * level3size, workgroupSize);

    int finalSum;
    kernel->in(a3wrapper);
    kernel->out(1, &finalSum);
    kernel->run_1d(level3size, level3size);
//    timer.timeCheck("finished 3-level reduce");

    EXPECT_EQ(sumViaCpu, finalSum);

    delete a1wrapper;
    delete a2wrapper;
    delete a3wrapper;
    delete[] a3;
    delete[] a2;
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(SLOW_testlocal, reduceviascratch_multipleworkgroups_ints_3levels) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "reduceViaScratch_multipleworkgroups_ints", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    const int numWorkgroups = workgroupSize;
    const int level3size = numWorkgroups / 4;
    const int N = workgroupSize * numWorkgroups * level3size;
    cout << "numworkgroups " << numWorkgroups << " workgroupsize " << workgroupSize << " N " << N << endl;
    int *myarray = new int[N];
    int sumViaCpu = 0;
    int localSumViaCpu = 0;
    int localSumViaCpu2 = 0;
    int *localSumsViaCpu = new int[numWorkgroups * level3size];
    memset(localSumsViaCpu, 0, sizeof(int)*numWorkgroups* level3size);
    for(int i = 0; i < N; i++) {
        myarray[i] = ((i + 7) * 3) % 50;
        sumViaCpu += myarray[i];
        if(i < workgroupSize) {
            localSumViaCpu += myarray[i];
        }
        if(i >= workgroupSize && i < workgroupSize * 2) {
            localSumViaCpu2 += myarray[i];
        }
        int workgroupId = i / workgroupSize;
        localSumsViaCpu[workgroupId] += myarray[i];
    }
    ASSERT_EQ(localSumViaCpu, localSumsViaCpu[0]);
    ASSERT_EQ(localSumViaCpu2, localSumsViaCpu[1]);
    ASSERT_NE(myarray[0], sumViaCpu);

//    Timer timer;

    CLWrapper *a1wrapper = cl->wrap(N, myarray);
    a1wrapper->copyToDevice();
//    timer.timeCheck("copied array to device");
    int *a2 = new int[numWorkgroups*level3size];
    CLWrapper *a2wrapper = cl->wrap(numWorkgroups * level3size, a2);
    kernel->in(a1wrapper);
    kernel->out(a2wrapper);
    kernel->localInts(workgroupSize);
    kernel->run_1d(N, workgroupSize);

    int *a3 = new int[numWorkgroups];
    CLWrapper *a3wrapper = cl->wrap(level3size, a3);
    kernel->in(a2wrapper);
    kernel->out(a3wrapper);
    kernel->localInts(workgroupSize);
    kernel->run_1d(workgroupSize * level3size, workgroupSize);

    int finalSum;
    kernel->in(a3wrapper);
    kernel->out(1, &finalSum);
    kernel->localInts(level3size);
    kernel->run_1d(level3size, level3size);
//    timer.timeCheck("finished 3-level reduce");

    EXPECT_EQ(sumViaCpu, finalSum);

    delete a1wrapper;
    delete a2wrapper;
    delete a3wrapper;
    delete[] a3;
    delete[] a2;
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(SLOW_testlocal, selfdot_3levels_withscratch) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernel("testlocal.cl", "selfdot_ints_withscratch");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    const int numWorkgroups = workgroupSize;
    const int level3size = numWorkgroups / 4;
    const int N = workgroupSize * numWorkgroups * level3size;
    cout << "numworkgroups " << numWorkgroups << " workgroupsize " << workgroupSize << " N " << N << endl;
    int *myarray = new int[N];
    for(int i = 0; i < N; i++) {
        myarray[i] = ((i + 7) * 3) % 5;
    }

//    Timer timer;

    CLWrapper *a1wrapper = cl->wrap(N, myarray);
    a1wrapper->copyToDevice();
//    timer.timeCheck("copied array to device");
    int *a2 = new int[numWorkgroups*level3size];
    CLWrapper *a2wrapper = cl->wrap(numWorkgroups * level3size, a2);
    kernel->in(a1wrapper);
    kernel->out(a2wrapper);
    kernel->localInts(workgroupSize);
    kernel->localInts(workgroupSize);
    kernel->run_1d(N, workgroupSize);
    cl->finish();

    int *a3 = new int[numWorkgroups];
    CLWrapper *a3wrapper = cl->wrap(level3size, a3);
    kernel->in(a2wrapper);
    kernel->out(a3wrapper);
    kernel->localInts(workgroupSize);
    kernel->localInts(workgroupSize);
    kernel->run_1d(workgroupSize * level3size, workgroupSize);
    cl->finish();

    int finalSum;
    kernel->in(a3wrapper);
    kernel->out(1, &finalSum);
    kernel->localInts(level3size);
    kernel->localInts(workgroupSize);
    kernel->run_1d(level3size, level3size);
//    timer.timeCheck("finished 3-level reduce");

    EXPECT_EQ(-1306309159, finalSum);

    delete a1wrapper;
    delete a2wrapper;
    delete a3wrapper;
    delete[] a3;
    delete[] a2;
    delete[]myarray;
    delete kernel;
    delete cl;
}

TEST(SLOW_testlocal, selfdot_3levels_withoutscratch) {
    EasyCL *cl = EasyCL::createForFirstGpuOtherwiseCpu();
    CLKernel *kernel = cl->buildKernelFromString(getKernel(), "selfdot_ints_withoutscratch", "");
    int workgroupSize = min(512, cl->getMaxWorkgroupSize());
    const int numWorkgroups = workgroupSize;
    const int level3size = numWorkgroups / 4;
    const int N = workgroupSize * numWorkgroups * level3size;
    cout << "numworkgroups " << numWorkgroups << " workgroupsize " << workgroupSize << " N " << N << endl;
    int *myarray = new int[N];
    for(int i = 0; i < N; i++) {
        myarray[i] = ((i + 7) * 3) % 5;
    }

//    Timer timer;

    CLWrapper *a1wrapper = cl->wrap(N, myarray);
    a1wrapper->copyToDevice();
//    timer.timeCheck("copied array to device");
    int *second = new int[N];
    CLWrapper *secondwrapper = cl->wrap(N, second);
    int *a2 = new int[numWorkgroups*level3size];
    CLWrapper *a2wrapper = cl->wrap(numWorkgroups * level3size, a2);
    kernel->in(a1wrapper);
    kernel->out(secondwrapper);
    kernel->out(a2wrapper);
    kernel->run_1d(N, workgroupSize);
    cl->finish();

    int *a3 = new int[numWorkgroups];
    CLWrapper *a3wrapper = cl->wrap(level3size, a3);
    kernel->in(a2wrapper);
    kernel->out(secondwrapper);
    kernel->out(a3wrapper);
    kernel->run_1d(workgroupSize * level3size, workgroupSize);
    cl->finish();

    int finalSum;
    kernel->in(a3wrapper);
    kernel->out(secondwrapper);
    kernel->out(1, &finalSum);
    kernel->run_1d(level3size, level3size);
//    timer.timeCheck("finished 3-level reduce");

    EXPECT_EQ(-1306309159, finalSum);

    delete a1wrapper;
    delete a2wrapper;
    delete a3wrapper;
    delete secondwrapper;
    delete[] a3;
    delete[] second;
    delete[] a2;
    delete[]myarray;
    delete kernel;
    delete cl;
}

static const char *getKernel() {
    // [[[cog
    // import stringify
    // stringify.stringify("source", "test/testlocal.cl")
    // ]]]
    // generated using cog, from test/testlocal.cl:
    const char * source =  
    "kernel void useLocal(int N, global float *inout, local float *_buffer) {\n" 
    "    if((int)get_global_id(0) < N) {\n" 
    "        inout[get_global_id(0)] += 1.0f;\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "kernel void notUseLocal(int N, global float *inout) {\n" 
    "    if((int)get_global_id(0) < N) {\n" 
    "        inout[get_global_id(0)] += 1.0f;\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "kernel void reduceGlobal(global float *inout) {\n" 
    "    // simply going to reduce inout values into inout[0]\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        if(localId < offset) {\n" 
    "            inout[localId] = inout[localId] + inout[localId + offset];\n" 
    "        }\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "kernel void reduceViaScratch(global float *inout, local float *scratch) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "\n" 
    "    scratch[localId] = inout[localId];\n" 
    "    barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        if(localId < offset) {\n" 
    "            scratch[localId] = scratch[localId] + scratch[localId + offset];\n" 
    "        }\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "    inout[localId] = scratch[localId];\n" 
    "}\n" 
    "\n" 
    "\n" 
    "kernel void reduceViaScratch_noif(global float *inout, local float *scratch) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "    scratch[localId] = inout[localId];\n" 
    "\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        scratch[localId] = localId < offset ? scratch[localId] + scratch[localId + offset] : scratch[localId];\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "    inout[localId] = scratch[localId];\n" 
    "}\n" 
    "\n" 
    "kernel void reduceViaScratch_noif2(global float *inout, local float *scratch) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "    scratch[localId] = inout[localId];\n" 
    "\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        scratch[localId] = scratch[localId] + scratch[localId + offset];\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "    inout[localId] = scratch[localId];\n" 
    "}\n" 
    "\n" 
    "kernel void reduceViaScratch_multipleworkgroups(global float *inout, global float *out, local float *scratch) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int globalId = get_global_id(0);\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "    int workgroupid = get_group_id(0);\n" 
    "\n" 
    "    scratch[localId] = inout[globalId];\n" 
    "    barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        if(localId < offset) {\n" 
    "            scratch[localId] = scratch[localId] + scratch[localId + offset];\n" 
    "        }\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "    inout[globalId] = scratch[localId];\n" 
    "    if(localId == 0) {\n" 
    "        out[workgroupid] = scratch[0];\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "kernel void reduceViaScratch_multipleworkgroups_ints(global int *inout, global int *out, local int *scratch) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int globalId = get_global_id(0);\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "    int workgroupid = get_group_id(0);\n" 
    "\n" 
    "    scratch[localId] = inout[globalId];\n" 
    "    barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        if(localId < offset) {\n" 
    "            scratch[localId] = scratch[localId] + scratch[localId + offset];\n" 
    "        }\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "    inout[globalId] = scratch[localId];\n" 
    "    if(localId == 0) {\n" 
    "        out[workgroupid] = scratch[0];\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "kernel void reduce_multipleworkgroups_ints_noscratch(global int *inout, global int *out) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int globalId = get_global_id(0);\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "    int workgroupid = get_group_id(0);\n" 
    "\n" 
    "//    scratch[localId] = inout[globalId];\n" 
    "    barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        if(localId < offset) {\n" 
    "            inout[globalId] = inout[globalId] + inout[globalId + offset];\n" 
    "        }\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "//    inout[globalId] = scratch[localId];\n" 
    "    if(localId == 0) {\n" 
    "        out[workgroupid] = inout[globalId];\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "kernel void selfdot_ints_withscratch(global int *in, global int *out, local int *scratch1, local int *scratch2) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int globalId = get_global_id(0);\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "    int workgroupid = get_group_id(0);\n" 
    "\n" 
    "    scratch1[localId] = in[globalId];\n" 
    "    barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    int sum = 0;\n" 
    "    int us = scratch1[localId];\n" 
    "    for(int i = 0; i < workgroupSize; i++) {\n" 
    "        sum += us * scratch1[i];\n" 
    "    }\n" 
    "    scratch2[localId] = sum;\n" 
    "    barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        if(localId < offset) {\n" 
    "            scratch2[localId] = scratch2[localId] + scratch2[localId + offset];\n" 
    "        }\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "//    inout[globalId] = scratch[localId];\n" 
    "    if(localId == 0) {\n" 
    "        out[workgroupid] = scratch2[0];\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "kernel void selfdot_ints_withoutscratch(global int *inout, global int *second, global int *out) {\n" 
    "    // simply going to reduce inout values into inout[0], via scratch, to test 'local'\n" 
    "    int globalId = get_global_id(0);\n" 
    "    int localId = get_local_id(0);\n" 
    "    int workgroupSize = get_local_size(0);\n" 
    "    int workgroupid = get_group_id(0);\n" 
    "\n" 
    "    const int workgroupOffset = workgroupid * workgroupSize;\n" 
    "\n" 
    "    int sum = 0;\n" 
    "    int us = inout[globalId];\n" 
    "    for(int i = 0; i < workgroupSize; i++) {\n" 
    "        sum += inout[workgroupOffset + i] * us;\n" 
    "    }\n" 
    "    second[globalId] = sum;\n" 
    "    barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {\n" 
    "        if(localId < offset) {\n" 
    "            second[globalId] = second[globalId] + second[globalId + offset];\n" 
    "        }\n" 
    "        barrier(CLK_LOCAL_MEM_FENCE);\n" 
    "    }\n" 
    "\n" 
    "    if(localId == 0) {\n" 
    "        out[workgroupid] = second[globalId];\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "\n" 
    "";
    // [[[end]]]
    return source;
}

