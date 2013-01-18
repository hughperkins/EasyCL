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
    // cout << "source: " << source << endl;
    const char *source_char = source.c_str();
    src_size = source.length();
    cl_program program = clCreateProgramWithSource(context, 1, &source_char, &src_size, &error);
    assert(error == CL_SUCCESS);

    // Builds the program
    error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // Shows the log
    char* build_log;
    size_t log_size;
    // First call to know the proper size
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    build_log = new char[log_size+1];
    // Second call to get the log
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    build_log[log_size] = '\0';
    if( log_size > 2 ) {
        cout << "build log: " << build_log << endl;
        }
    delete[] build_log;

    checkError(error);

    // Extracting the kernel
    cl_kernel kernel = clCreateKernel(program, kernelname.c_str(), &error);
    checkError(error);
    return new CLKernel(this, kernel);
}

