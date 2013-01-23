kernel void test(const int anint, const float afloat, global int *intout, global float *floatout ) {
    const int globalid = get_global_id(0);
    intout[globalid] = anint + globalid;
    floatout[globalid] = afloat + globalid;
}

