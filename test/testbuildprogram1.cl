
__kernel void first(int a,int b,__global int *c){
  c[0] = a;
  c[1] = b;
}

__kernel void second(int a,int b,__global int *c){
  c[0] = a;
  c[1] = b;
}

__kernel void third(int a,int b,__global int *c){
  c[0] = a;
  c[1] = b;
}
