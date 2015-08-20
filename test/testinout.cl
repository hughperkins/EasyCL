kernel void test(global float *inout) {
    const int globalid = get_global_id(0);
    inout[globalid] = inout[globalid] + 7;
}

