__kernel void add(__global const float* a, __global const float*  b, __global float* c)
{
int idx=get_global_id(0);
c[idx]=a[idx]+b[idx];
}