kernel void test(global float *in, global float *out) {
    const int globalid = get_global_id(0);
    out[globalid] = in[globalid] + 7;
}

kernel void testuchars(global unsigned char *in, global unsigned char *out) {
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
//    while(n < 1000000) {
    while(n < 10001) {
        sum = (sum + in[n % 47]) % (103070 * 7);
        n++;
    }
    out[globalid] = sum;
}

kernel void test_read(const int one,  const int two, global int *out) {
    const int globalid = get_global_id(0);
    int sum = 0;
    int n = 0;
    while(n < 100000) {
        sum = (sum + one) % 1357 * two;
        n++;
    }
//    out[globalid+2048] = sum;
//    out[globalid] = sum;
//    out[0] = 44;
    out[globalid] = sum;
   // out[0] = globalid > out[0] ? globalid : out[0];
//    out[globalid] = 8827;
}

