#ifndef CLCC_OPTIONS_HPP_INCLUDED
#define CLCC_OPTIONS_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Contributions by:   Fredrik Orderud (https://sourceforge.net/users/fredrior/)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

//! \file options.hpp
//! \brief  Defines option class that handles all command-line %options.

#include "clew.h"

#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

enum
{   CLCC_ALL_PLATFORMS  =   -1
,   CLCC_ALL_DEVICES    =   -1
};

//! \brief  Class that handles command-line options
class options : boost::noncopyable
{
    public:
                                        //! \brief  Constructor
                                        options(int argc, char* argv[]);
                                        //! \brief  Destructor
                                       ~options();

                                        //! \brief  Return path to OpenCL dynamic library
        const std::string&              clpath() const                          {return clpath_;}
                                        //! \brief  Returns \e true if the user requested info
        bool                            info() const                            {return info_;  }
                                        //! \brief  Returns \e true if an OpenCL source file was specified on the command line
        bool                            build() const                           {return build_; }
                                        //! \brief  Returns the selected device type
        cl_device_type                  device_type() const                     {return device_type_;}
                                        //! \brief  Returns the OpenCL compiler options specified on the command line
        const std::string&              cloptions() const                       {return cloptions_; }
                                        //! \brief  Returns the OpenCL source files specified on the command line
        const std::vector<std::string>& input_files() const;
                                        //! \brief  Returns the filename for output binaries
        const std::string&              output_file() const;
                                        //! \brief  Returns a platform name filter used to select a platform
        const std::string&              platform_name_filter() const;
                                        //! \brief  Returns a device name filter used to select a device
        const std::string&              device_name_filter() const;
                                        //! \brief  Returns the index of the platform to target
        int                             platform_index() const;
                                        //! \brief  Returns the index of the device to target
        int                             device_index() const;
                                        //! \brief  Add CLCC headers to output binaries
        bool                            add_headers() const                     {return add_headers_;}


    private:
        //! \brief  Short-hand variable map type
        typedef boost::program_options::variables_map   variables_map_t;
        
                            //! \brief  Variable map
        variables_map_t     variables_;
                            //! \brief  Path to OpenCL dynamic library
        std::string         clpath_;
                            //! \brief  Information request flag
        bool                info_;
                            //! \brief  Build flag
        bool                build_;
                            //! \brief  Device type to build for
        cl_device_type      device_type_;
                            //! \brief  OpenCL compiler options
        std::string         cloptions_;
                            //! \brief  Platform name filter
        std::string         platform_name_filter_;
                            //! \brief  Device name filter
        std::string         device_name_filter_;
                            //! \brief  Name out binary output file
        std::string         output_file_;
                            //! \brief  Index of platform to build for or CLCC_ALL_PLATFORMS
        int                 platform_index_;
                            //! \brief  Index of device to build for or CLCC_ALL_DEVICES
        int                 device_index_;
                            //! \brief  Add CLCC headers to output binaries
        bool                add_headers_;
};

#endif  //  CLCC_OPTIONS_HPP_INCLUDED
