kernel void test(global int *in, global int *out, global int *inout) {
    const int globalid = get_global_id(0);
    inout[globalid] = inout[globalid] + 7;
    out[globalid] = in[globalid] + 5;
    if(globalid == 2) {
        out[globalid] = 26;
        inout[globalid] = 34;
    }
}

