kernel void test(const int aint, const unsigned auint,
    long along, unsigned long aulong,
  const float afloat,
  global int *intout, 
  global unsigned int *uintout, 
  global long *longout, 
  global unsigned long *ulongout, 
  global float *floatout
 ) {
    const int globalid = get_global_id(0);

    intout[globalid] = aint + globalid;
    uintout[globalid] = auint + globalid;
    longout[globalid] = along + globalid;
    ulongout[globalid] = aulong + globalid;

    floatout[globalid] = afloat + globalid;
}

