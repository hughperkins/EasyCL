kernel void reduceGlobal( global float *inout ) {
    // simply going to reduce inout values into inout[0]
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);

    for( int offset = ( workgroupSize >> 1 ); offset > 0; offset >>= 1 ) {
        if( localId < offset ) {
            inout[localId] = inout[localId] + inout[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}

kernel void reduceViaScratch( global float *inout, local float *scratch ) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    scratch[localId] = inout[localId];

    for( int offset = ( workgroupSize >> 1 ); offset > 0; offset >>= 1 ) {
        if( localId < offset ) {
            scratch[localId] = scratch[localId] + scratch[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[localId] = scratch[localId];
}


kernel void reduceViaScratch_noif( global float *inout, local float *scratch ) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    scratch[localId] = inout[localId];

    for( int offset = ( workgroupSize >> 1 ); offset > 0; offset >>= 1 ) {
        scratch[localId] = localId < offset ? scratch[localId] + scratch[localId + offset] : scratch[localId];
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[localId] = scratch[localId];
}

kernel void reduceViaScratch_noif2( global float *inout, local float *scratch ) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    scratch[localId] = inout[localId];

    for( int offset = ( workgroupSize >> 1 ); offset > 0; offset >>= 1 ) {
        scratch[localId] = scratch[localId] + scratch[localId + offset];
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[localId] = scratch[localId];
}

kernel void reduceViaScratch_multipleworkgroups( global float *inout, global float *out, local float *scratch ) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

    scratch[localId] = inout[globalId];
    barrier(CLK_LOCAL_MEM_FENCE);
    for( int offset = ( workgroupSize >> 1 ); offset > 0; offset >>= 1 ) {
        if( localId < offset ) {
            scratch[localId] = scratch[localId] + scratch[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[globalId] = scratch[localId];
    if( localId == 0 ) {
        out[workgroupid] = scratch[0];
    }
}

kernel void reduceViaScratch_multipleworkgroups_ints( global int *inout, global int *out, local int *scratch ) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

    scratch[localId] = inout[globalId];
    barrier(CLK_LOCAL_MEM_FENCE);
    for( int offset = ( workgroupSize >> 1 ); offset > 0; offset >>= 1 ) {
        if( localId < offset ) {
            scratch[localId] = scratch[localId] + scratch[localId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    inout[globalId] = scratch[localId];
    if( localId == 0 ) {
        out[workgroupid] = scratch[0];
    }
}

kernel void reduce_multipleworkgroups_ints_noscratch( global int *inout, global int *out ) {
    // simply going to reduce inout values into inout[0], via scratch, to test 'local'
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int workgroupSize = get_local_size(0);
    int workgroupid = get_group_id(0);

//    scratch[localId] = inout[globalId];
    barrier(CLK_LOCAL_MEM_FENCE);
    for( int offset = ( workgroupSize >> 1 ); offset > 0; offset >>= 1 ) {
        if( localId < offset ) {
            inout[globalId] = inout[globalId] + inout[globalId + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

//    inout[globalId] = scratch[localId];
    if( localId == 0 ) {
        out[workgroupid] = inout[globalId];
    }
}



