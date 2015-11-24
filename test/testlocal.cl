kernel void useLocal(int N, global float *inout, local float *_buffer) {
    if((int)get_global_id(0) < N) {
        inout[get_global_id(0)] += 1.0f;
    }
}

kernel void notUseLocal(int N, global float *inout) {
    if((int)get_global_id(0) < N) {
        inout[get_global_id(0)] += 1.0f;
    }
}

kernel void reduceGlobal(global float *inout) {
    // simply going to reduce inout values into inout[0]
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);

    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        if(localId < offset) {
            inout[localId] = inout[localId] + inout[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}

kernel void reduceViaScratch(global float *inout, local float *scratch) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);

    scratch[localId] = inout[localId];
    barrier(CLK_LOCAL_MEM_FENCE);

    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        if(localId < offset) {
            scratch[localId] = scratch[localId] + scratch[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[localId] = scratch[localId];
}


kernel void reduceViaScratch_noif(global float *inout, local float *scratch) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    scratch[localId] = inout[localId];

    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        scratch[localId] = localId < offset ? scratch[localId] + scratch[localId + offset] : scratch[localId];
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[localId] = scratch[localId];
}

kernel void reduceViaScratch_noif2(global float *inout, local float *scratch) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    scratch[localId] = inout[localId];

    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        scratch[localId] = scratch[localId] + scratch[localId + offset];
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[localId] = scratch[localId];
}

kernel void reduceViaScratch_multipleworkgroups(global float *inout, global float *out, local float *scratch) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

    scratch[localId] = inout[globalId];
    barrier(CLK_LOCAL_MEM_FENCE);
    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        if(localId < offset) {
            scratch[localId] = scratch[localId] + scratch[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[globalId] = scratch[localId];
    if(localId == 0) {
        out[workgroupid] = scratch[0];
    }
}

kernel void reduceViaScratch_multipleworkgroups_ints(global int *inout, global int *out, local int *scratch) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

    scratch[localId] = inout[globalId];
    barrier(CLK_LOCAL_MEM_FENCE);
    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        if(localId < offset) {
            scratch[localId] = scratch[localId] + scratch[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[globalId] = scratch[localId];
    if(localId == 0) {
        out[workgroupid] = scratch[0];
    }
}

kernel void reduce_multipleworkgroups_ints_noscratch(global int *inout, global int *out) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

//    scratch[localId] = inout[globalId];
    barrier(CLK_LOCAL_MEM_FENCE);
    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        if(localId < offset) {
            inout[globalId] = inout[globalId] + inout[globalId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

//    inout[globalId] = scratch[localId];
    if(localId == 0) {
        out[workgroupid] = inout[globalId];
    }
}

kernel void selfdot_ints_withscratch(global int *in, global int *out, local int *scratch1, local int *scratch2) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

    scratch1[localId] = in[globalId];
    barrier(CLK_LOCAL_MEM_FENCE);
    int sum = 0;
    int us = scratch1[localId];
    for(int i = 0; i < workgroupSize; i++) {
        sum += us * scratch1[i];
    }
    scratch2[localId] = sum;
    barrier(CLK_LOCAL_MEM_FENCE);
    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        if(localId < offset) {
            scratch2[localId] = scratch2[localId] + scratch2[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

//    inout[globalId] = scratch[localId];
    if(localId == 0) {
        out[workgroupid] = scratch2[0];
    }
}

kernel void selfdot_ints_withoutscratch(global int *inout, global int *second, global int *out) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

    const int workgroupOffset = workgroupid * workgroupSize;

    int sum = 0;
    int us = inout[globalId];
    for(int i = 0; i < workgroupSize; i++) {
        sum += inout[workgroupOffset + i] * us;
    }
    second[globalId] = sum;
    barrier(CLK_LOCAL_MEM_FENCE);
    for(int offset = (workgroupSize >> 1); offset > 0; offset >>= 1) {
        if(localId < offset) {
            second[globalId] = second[globalId] + second[globalId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(localId == 0) {
        out[workgroupid] = second[globalId];
    }
}


