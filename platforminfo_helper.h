#pragma once

#include <algorithm>
#include <string>

void printPlatformInfoString( string valuename, cl_platform_id platformId, cl_platform_info name ) {
    char buffer[256];
    clGetPlatformInfo(platformId, name, 256, buffer, 0);
    cout << valuename << ": " << buffer << endl;    
}

void printPlatformInfo( string valuename, cl_platform_id platformId, cl_platform_info name ) {
    cl_ulong somelong = 0;
    clGetPlatformInfo(platformId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << somelong << endl;    
}


