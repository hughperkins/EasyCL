kernel void test(global float *in, global float *out) {
    const int globalid = get_global_id(0);
    out[globalid] = in[globalid] + 7;
}

kernel void test_int(global int *in, global int *out) {
    const int globalid = get_global_id(0);
    out[globalid] = in[globalid] + 7;
}

kernel void test_stress(global const int *in, global int *out) {
    const int globalid = get_global_id(0);
    int sum = 0;
    int n = 0;
   // make it do some work....
    while( n < 1000000 ) {
        sum = (sum + in[n]) % 1000000;
        n++;
    }
    out[globalid] = sum;
//    out[globalid] = in[globalid] + 7;
}


