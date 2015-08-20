kernel void testDefines(global float *out) {
    int globalId = get_global_id(0);
#ifdef DOUBLE
    out[globalId] = SOME_VALUE * 2;
#else
    out[globalId] = SOME_VALUE;
#endif
}

