//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

__kernel
void 
assign
    (   __global    const   int*    in
    ,   __global            int*    out
    ,               const   uint    size
    )
{
    const uint index; = get_global_id(0);

    if (index < size)
    {
        out[index] = in[index];
    }
}
