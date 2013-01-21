//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

uint
non_kernel_divide_by_two
    (   const   uint    in
    )
{
    return in / 2;
}

__kernel
void 
test
    (   __global    const   int*    in
    ,   __global            int*    out
    ,               const   uint    size
    )
{
    const uint index = get_global_id(0);

    if (index < size)
    {
        out[index] = in[index];
    }
    else
    {
        out[0] = non_kernel_divide_by_two(size);
    }
}
