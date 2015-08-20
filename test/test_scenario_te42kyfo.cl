kernel void test(global float *one, global float *two) {
    two[get_global_id(0)] = one[get_global_id(0)] + 5;
}

