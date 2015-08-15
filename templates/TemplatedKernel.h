// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <vector>

class EasyCL;
class CLKernel;
class LuaTemplater;

#include "EasyCL_export.h"

#define VIRTUAL virtual
#define STATIC static

// idea of htis is that it will behave a bit like the templates in CUDA, ie you give it a set of template parameters, and if that kernel has
// already been built, for those parameters, then uses that, otherwise bulids for new parameters
// works by storing hte built kernels in a map, keyed on kernel file path, kernel method name, and template parameter names and values
class EasyCL_EXPORT TemplatedKernel {
public:
    EasyCL *cl;
    LuaTemplater *templater;

    // [[[cog
    // import cog_addheaders
    // cog_addheaders.addv2()
    // ]]]
    // generated, using cog:

    public:
    TemplatedKernel(EasyCL *cl);
    ~TemplatedKernel();
    TemplatedKernel &set(std::string name, int value);
    TemplatedKernel &set(std::string name, float value);
    TemplatedKernel &set(std::string name, std::string value);
    TemplatedKernel &set(std::string name, std::vector< std::string > &value);
    TemplatedKernel &set(std::string name, std::vector< int > &value);
    TemplatedKernel &set(std::string name, std::vector< float > &value);
    CLKernel *buildKernel(std::string uniqueName, std::string filename, std::string templateSource, std::string kernelName);
    CLKernel *buildKernel(std::string uniqueName, std::string filename, std::string templateSource, std::string kernelName, bool useKernelStore);
    std::string getRenderedKernel(std::string templateSource);

    private:
    CLKernel *_buildKernel(std::string uniqueName, std::string filename, std::string templateSource, std::string kernelName, bool useKernelStore);

    // [[[end]]]
};

