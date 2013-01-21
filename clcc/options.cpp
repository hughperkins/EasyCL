//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Contributions by:   
//      Fredrik Orderud (https://sourceforge.net/users/fredrior/)
//      Ian Cullinan    (https://sourceforge.net/users/cibyr/)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

//! \file options.cpp
//! \brief  Implements option class member functions.


#include "options.hpp"

#include "version.hpp"
#include "clexception.hpp"

#include <iostream>

namespace po = boost::program_options;

using namespace std;

#ifdef _WIN32
#define CLCC_OPENCL_DYNLIB_NAME "opencl.dll"
#endif

#ifdef __APPLE__
#define CLCC_OPENCL_DYNLIB_NAME "/Library/Frameworks/OpenCL.framework/OpenCL"
#endif

#ifdef __LINUX__
#define CLCC_OPENCL_DYNLIB_NAME "libOpenCL.so";
#endif

static const char* opencl_lib_default = CLCC_OPENCL_DYNLIB_NAME;    //!<    The default dynamic library name to open, its value is platform-specific
static const char* opencl_options = "";                             //!<    The default OpenCL compiler options
static const char* opencl_output_file_default = "cl.out";           //!<    The default output filename

//! \param argc number of arguments
//! \param argv array of zero-terminated strings containing the actual arguments
//!
//! The constructor will parse and process all arguments. If an error occurs, it
//! will throw an exception.
options::options
    (   int     argc
    ,   char*   argv[]
    )
    :   variables_()
    ,   clpath_()
    ,   info_()
    ,   build_(false)
    ,   device_type_(CL_DEVICE_TYPE_ALL)
    ,   cloptions_()
    ,   platform_name_filter_()
    ,   device_name_filter_()
    ,   output_file_()
    ,   platform_index_(-1)
    ,   device_index_(-1)
    ,   add_headers_(false)
{
    //  Options visible to the user
    po::options_description visible_options("Allowed options");
    visible_options.add_options()
        ("help,h"           ,                                                                                           "produce help message"                  )
        ("version,v"        ,                                                                                           "show version number"                   )
        ("info,i"           ,                                                                                           "list platforms and devices"            )
        ("cloptions"        ,   po::value<string>(&cloptions_           )->default_value(opencl_options             ),  "opencl compiler options"               )
        ("clpath"           ,   po::value<string>(&clpath_              )->default_value(opencl_lib_default         ),  "path to opencl dynamic library"        )
        ("output_file,o"    ,   po::value<string>(&output_file_         )->default_value(opencl_output_file_default ),  "path to output file"                   )
        ("platform_filter"  ,   po::value<string>(&platform_name_filter_)                                            ,  "case-insensitive platform name filter" )
        ("device_filter"    ,   po::value<string>(&device_name_filter_  )                                            ,  "case-insensitive device name filter"   )
        ("device_type"      ,   po::value<string>()->default_value("ALL")                                            ,  "opencl device type to build for\n\n"   
                                                                                                                        "Options:\n\n"
                                                                                                                        "  ALL - all device types\n"
                                                                                                                        "  CPU - cpu device type\n"
                                                                                                                        "  GPU - gpu device type\n"
                                                                                                                        "  ACCELERATOR - accelerator device type\n")
        ("platform_index,p" ,   po::value<int  >(&platform_index_)                                                   ,  "zero-based platform index"             )
        ("device_index,d"   ,   po::value<int  >(&device_index_)                                                     ,  "zero-based device index"               )
        ("add_headers"      ,                                                                                           "add CLCC headers to output binaries\n(this option is required when building for multiple platforms/devices)"   )
        ;

    //  Options hidden to the user and to be transformed into positional options
    po::options_description  hidden_options("Hidden options");
    hidden_options.add_options()
        ("input-file"   ,   po::value< vector<string> >()                                       ,   "input file");

    //  All available options
    po::options_description all_options;
    all_options.add(visible_options).add(hidden_options);

    //  Set "input-file" option as positional
    po::positional_options_description p;
    p.add("input-file", -1);

    //  Parse all available options
    po::store(po::command_line_parser(argc, argv).options(all_options).positional(p).run(), variables_);
    po::notify(variables_);


    //  Process input
    if  (   (argc == 1)
        ||  (variables_.count("help"))
        )
    {
        cout << "Usage: clcc [options] file(s)..." << endl;
        cout << visible_options << endl;
    }

    if (variables_.count("version"))
    {
        cout    <<  "clcc version "
                <<  CLCC_VERSION_MAJOR  << '.' 
                <<  CLCC_VERSION_MINOR  << '.'
                <<  CLCC_VERSION_PATCH  << " (build: "
                <<  CLCC_VERSION_BUILD  << ", "
                <<  CLCC_BUILD_DATETIME << ")\n"
                ;
    }

    info_        = (variables_.count("info"       ) != 0);
    build_       = (variables_.count("input-file" ) != 0);
    add_headers_ = (variables_.count("add_headers") != 0);

    if (platform_index_ < -1)
    {
        BOOST_THROW_EXCEPTION(cl::exception("platform_index cannot be negative"));
    }

    if (device_index_ < -1)
    {
        BOOST_THROW_EXCEPTION(cl::exception("device_index cannot be negative"));
    }

    if (variables_.count("device_type"))
    {
        std::string type = variables_["device_type"].as<std::string>();

                if (type == "ACCELERATOR" )   device_type_ = CL_DEVICE_TYPE_ACCELERATOR;
        else    if (type == "GPU"         )   device_type_ = CL_DEVICE_TYPE_GPU;
        else    if (type == "CPU"         )   device_type_ = CL_DEVICE_TYPE_CPU;
        else    if (type == "ALL"         )   device_type_ = CL_DEVICE_TYPE_ALL;
        else    throw runtime_error("error: unrecognized device_type option");
    }
}

options::~options()
{
}

//! \return a vector of strings containing all file names that need to be compiled
const vector<string>& 
options::input_files() const
{
    return variables_["input-file"].as< vector<string> >();
}

const std::string&
options::output_file() const
{
    return output_file_;
}

const std::string&
options::platform_name_filter() const
{
    return platform_name_filter_;
}

const std::string&
options::device_name_filter() const
{
    return device_name_filter_;
}

int
options::platform_index() const
{
    return platform_index_;
}

int
options::device_index() const
{
    return device_index_;
}
