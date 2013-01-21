// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>
using namespace std;

#include "OpenCLHelper.h"

#include "CLArray1d.h"
#include "CLKernel.h"

CLArray1d *OpenCLHelper::array1d(int N ) {
    return new CLArray1d( N, this );
}

CLKernel *OpenCLHelper::buildKernel( string kernelfilepath, string kernelname ) {
    size_t src_size = 0;
    std::string path = kernelfilepath.c_str();
    std::string source = getFileContents(path);
    const char *source_char = source.c_str();
    src_size = strlen( source_char );
    cl_program program = clCreateProgramWithSource(context, 1, &source_char, &src_size, &error);
    assert(error == CL_SUCCESS);

//    error = clBuildProgram(program, 1, &device, "-cl-opt-disable", NULL, NULL);
    error = clBuildProgram(program, 1, &device, 0, NULL, NULL);

    char* build_log;
    size_t log_size;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    build_log = new char[log_size+1];
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    build_log[log_size] = '\0';
    if( log_size > 2 ) {
        cout << "build log: " << build_log << endl;
        }
    delete[] build_log;
    checkError(error);

    cl_kernel kernel = clCreateKernel(program, kernelname.c_str(), &error);
    switch( error ) {
        case CL_SUCCESS:
            break;
        case -46:
            cout << "Invalid kernel name, code -46 " << kernelfilepath << " " << kernelname << endl;
            exit(-1);
            break;
        default:
            cout << "Something went wrong with clCreateKernel, code " << error << endl;
            exit(-1);
            break;
    }
    checkError(error);
    return new CLKernel(this, kernel);
}

