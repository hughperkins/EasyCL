#ifndef CLCC_OUTPUT_HPP_INCLUDED
#define CLCC_OUTPUT_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

//! \file output.hpp
//! \brief CLCC object file header definitions

#include "version.hpp"

#ifdef _MSC_VER
    #pragma pack(push, 1)
#endif

//! \brief  Device header
struct clcc_binary_device_header
{
                    unsigned long   type;                   //!<    Device type (equivalent to cl_device_type)
                    int             binary_length;          //!<    Length of program binary in bytes

                    int             name_length;            //!<    Length of name string of this platform, this includes a terminating nul-character
                    int             vendor_length;          //!<    Length of vendor string of this platform, this includes a terminating nul-character
                    int             driver_version_length;  //!<    Length of driver version string of this platform, this includes a terminating nul-character
                    int             device_version_length;  //!<    Length of device version string of this platform, this includes a terminating nul-character
};

//! \brief  Platform header
struct clcc_binary_platform_header
{
                    int             num_devices;    //!<    Number of devices in this platform

                    int             name_length;    //!<    Length of name string of this platform, this includes a terminating nul-character
                    int             vendor_length;  //!<    Length of vendor string of this platform, this includes a terminating nul-character
                    int             version_length; //!<    Length of version string of this platform, this includes a terminating nul-character
};

//! \brief  Object file header
struct clcc_binary_header
{   
    //  File tag 'clcc' for Big Endian files, 'cclc' for Little Endian files
                    int             tag             ;

    //  Version information, Little Endian 16-bit integers
                    int             version_major   ;
                    int             version_minor   ;
                    int             version_revision;
                    int             version_build   ;

    //  Content information
                    int             platform_count  ;  //!<    Number of platforms contained in file
};

//! \todo   Write documentation on object file format
//! \todo   Write a loader for the format that puts it into a form ready to be processed by clCreateProgramWithBinary()
//
//  CLCC OBJECT FILE FORMAT
//
//  BINARY_HEADER
//      tag
//      version_major
//      version_minor
//      version_revision
//      version_build
//      platform count
//          PLATFORM_HEADER                             (platform 0)
//              device count
//              name string length
//              vendor string length
//              version string length
//
//              name string
//              vendor string
//              version string
//
//              DEVICE_HEADER                           (device a)
//                  type                
//                  binary_length
//                  
//                  name string length
//                  vendor string length
//                  driver version string length
//                  device version string length
//
//                  binary
//
//                  name string length
//                  vendor string length
//                  driver version string length
//                  device version string length
//
//              DEVICE_HEADER                           (device b)
//                  <device data>                       (... etc ...)
//          
//          PLATFORM_HEADER                             (platform 1)
//              <platform data>                         (... etc ...)
//
//  (etcetera)

#ifdef _MSC_VER
    #pragma pack(pop)
#endif

#endif  //  CLCC_OUTPUT_HPP_INCLUDED
