// Copyright Hugh Perkins 2013 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "CLArrayInt.h"
#include "CLArrayFloat.h"

#include "EasyCL_export.h"

class EasyCL_EXPORT CLKernel {
    EasyCL *easycl; // NOT owned by this object, dont delete!
    cl_program program;
    cl_kernel kernel;
    cl_int error;

    int nextArg;

#ifdef _WIN32
#pragma warning(disable: 4251)
#endif
	std::vector<cl_mem> buffers;

	std::vector<int> inputArgInts;
	std::vector<float> inputArgFloats;

	std::vector<cl_mem> outputArgBuffers;
	std::vector<void *> outputArgPointers;
	std::vector<size_t> outputArgSizes;

	template<typename T>
	static std::string toString(T val);
#ifdef _WIN32
#pragma warning(default: 4251)
#endif

public:

	CLKernel(EasyCL *easycl, cl_program program, cl_kernel kernel);
	CLKernel(const CLKernel &kernel);
	CLKernel &operator=(const CLKernel &kernel);
	~CLKernel();

    CLKernel *input( CLArray *clarray1d );
    CLKernel *inout( CLArray *clarray1d );
    CLKernel *output( CLArray *clarray1d );

    CLKernel *in( CLArray *clarray1d ) { return input( clarray1d ); }
    CLKernel *out( CLArray *clarray1d ) { return output( clarray1d ); }

    CLKernel *input( CLWrapper *wrapper );
    CLKernel *output( CLWrapper *wrapper );
    CLKernel *inout( CLWrapper *wrapper );

    CLKernel *in( CLWrapper *wrapper ) { return input( wrapper ); }
    CLKernel *out( CLWrapper *wrapper ) { return output( wrapper ); }

	CLKernel *localFloats(int count);
	CLKernel *localInts(int count);
	CLKernel *local(int N);

    template<typename T> CLKernel *input(int N, const T *data);
    template<typename T> CLKernel *in(int N, const T *data);
	CLKernel *input(int value);
	CLKernel *in(int value);
	CLKernel *input(float value);
	CLKernel *in(float value);
    template<typename T> CLKernel *output(int N, T *data);
    template<typename T> CLKernel *out(int N, T *data);
    template<typename T> CLKernel *inout(int N, T *data);
	void run_1d(int global_worksize, int local_worksize);
	void run(int ND, const size_t *global_ws, const size_t *local_ws);
};

