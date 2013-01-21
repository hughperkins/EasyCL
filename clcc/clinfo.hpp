#ifndef CLCC_CLINFO_HPP_INCLUDED
#define CLCC_CLINFO_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

//! \file clinfo.hpp
//! \brief OpenCL information classes header

#include "clew.h"

#include <boost/scoped_array.hpp>

#include <string>
#include <iostream>

#ifndef CLCC_GENERATE_DOCUMENTATION
//! \brief  Convert CL device type enum value to a string constant
#define CLDEVICE_TYPE_ENUM2STRING(e)                                            \
    (   e == CL_DEVICE_TYPE_ALL             ?   "CL_DEVICE_TYPE_ALL"            \
    :   e == CL_DEVICE_TYPE_CPU             ?   "CL_DEVICE_TYPE_CPU"            \
    :   e == CL_DEVICE_TYPE_GPU             ?   "CL_DEVICE_TYPE_GPU"            \
    :   e == CL_DEVICE_TYPE_ACCELERATOR     ?   "CL_DEVICE_TYPE_ACCELERATOR"    \
    :   e == CL_DEVICE_TYPE_DEFAULT         ?   "CL_DEVICE_TYPE_DEFAULT"        \
    :   "*** BAD DEVICE TYPE ***"                                               \
    )
#endif  //  CLCC_GENERATE_DOCUMENTATION

//! \brief namespace for all CL utility classes and functions
namespace cl
{
    //! \brief  This class represents the information about a specific OpenCL %device
    //!
    //! It contains all information as provided by the driver and can be used to
    //! dynamically tune software for optimal performance on this specific %device.
    class device
    {
        public:
                                                        device(cl_device_id device_id);
                                                        device(const device& device);
                    device&                             operator=(const device& device);
                                                       ~device();

                    cl_device_id                        device_id                       () const    {return device_id_;}

                    cl_device_type                      device_type                     () const    {return device_type_;}
                    cl_uint                             vendor_id                       () const    {return vendor_id_;}
                    cl_uint                             max_compute_units               () const    {return max_compute_units_;}
                    cl_uint                             max_work_item_dimensions        () const    {return max_work_item_dimensions_;}
                    const ::size_t*                     max_work_item_sizes             () const    {return max_work_item_sizes_.get();}
                    ::size_t                            max_work_group_size             () const    {return max_work_group_size_;}
                    cl_uint                             preferred_vector_width_char     () const    {return preferred_vector_width_char_;}
                    cl_uint                             preferred_vector_width_short    () const    {return preferred_vector_width_short_;}
                    cl_uint                             preferred_vector_width_int      () const    {return preferred_vector_width_int_;}
                    cl_uint                             preferred_vector_width_long     () const    {return preferred_vector_width_long_;}
                    cl_uint                             preferred_vector_width_float    () const    {return preferred_vector_width_float_;}
                    cl_uint                             preferred_vector_width_double   () const    {return preferred_vector_width_double_;}
                    cl_uint                             max_clock_frequency             () const    {return max_clock_frequency_;}
                    cl_uint                             address_bits                    () const    {return address_bits_;}
                    cl_ulong                            max_mem_alloc_size              () const    {return max_mem_alloc_size_;}
                    cl_bool                             image_support                   () const    {return image_support_;}
                    cl_uint                             max_read_image_args             () const    {return max_read_image_args_;}
                    cl_uint                             max_write_image_args            () const    {return max_write_image_args_;}
                    ::size_t                            image2d_max_width               () const    {return image2d_max_width_;}
                    ::size_t                            image2d_max_height              () const    {return image2d_max_height_;}
                    ::size_t                            image3d_max_width               () const    {return image3d_max_width_;}
                    ::size_t                            image3d_max_height              () const    {return image3d_max_height_;}
                    ::size_t                            image3d_max_depth               () const    {return image3d_max_depth_;}
                    cl_uint                             max_samplers                    () const    {return max_samplers_;}
                    ::size_t                            max_parameter_size              () const    {return max_parameter_size_;}
                    cl_uint                             mem_base_addr_align             () const    {return mem_base_addr_align_;}
                    cl_uint                             min_data_type_align_size        () const    {return min_data_type_align_size_;}
                    cl_device_fp_config                 single_fp_config                () const    {return single_fp_config_;}
                    cl_device_mem_cache_type            global_mem_cache_type           () const    {return global_mem_cache_type_;}
                    cl_uint                             global_mem_cacheline_size       () const    {return global_mem_cacheline_size_;}
                    cl_ulong                            global_mem_cache_size           () const    {return global_mem_cache_size_;}
                    cl_ulong                            global_mem_size                 () const    {return global_mem_size_;}
                    cl_ulong                            max_constant_buffer_size        () const    {return max_constant_buffer_size_;}
                    cl_uint                             max_constant_args               () const    {return max_constant_args_;}
                    cl_device_local_mem_type            local_mem_type                  () const    {return local_mem_type_;}
                    cl_ulong                            local_mem_size                  () const    {return local_mem_size_;}
                    cl_bool                             error_correction_support        () const    {return error_correction_support_;}
                    ::size_t                            profiling_timer_resolution      () const    {return profiling_timer_resolution_;}
                    cl_bool                             endian_little                   () const    {return endian_little_;}
                    cl_bool                             available                       () const    {return available_;}
                    cl_bool                             compiler_available              () const    {return compiler_available_;}
                    cl_device_exec_capabilities         execution_capabilities          () const    {return execution_capabilities_;}
                    cl_command_queue_properties         queue_properties                () const    {return queue_properties_;}
                    cl_platform_id                      platform_id                     () const    {return platform_id_;}
                    const std::string&                  name                            () const    {return name_;}
                    const std::string&                  vendor                          () const    {return vendor_;}
                    const std::string&                  driver_version                  () const    {return driver_version_;}
                    const std::string&                  profile                         () const    {return profile_;}
                    const std::string&                  device_version                  () const    {return device_version_;}
                    const std::string&                  extensions                      () const    {return extensions_;}

                    bool                                extension_supported             (const std::string& extension_name) const;

        private:
                    cl_device_id                        device_id_;
                    cl_device_type                      device_type_;
                    cl_uint                             vendor_id_;
                    cl_uint                             max_compute_units_;
                    cl_uint                             max_work_item_dimensions_;
                    boost::scoped_array< ::size_t>      max_work_item_sizes_;
                    ::size_t                            max_work_group_size_;
                    cl_uint                             preferred_vector_width_char_;
                    cl_uint                             preferred_vector_width_short_;
                    cl_uint                             preferred_vector_width_int_;
                    cl_uint                             preferred_vector_width_long_;
                    cl_uint                             preferred_vector_width_float_;
                    cl_uint                             preferred_vector_width_double_;
                    cl_uint                             max_clock_frequency_;
                    cl_uint                             address_bits_;
                    cl_ulong                            max_mem_alloc_size_;
                    cl_bool                             image_support_;
                    cl_uint                             max_read_image_args_;
                    cl_uint                             max_write_image_args_;
                    ::size_t                            image2d_max_width_;
                    ::size_t                            image2d_max_height_;
                    ::size_t                            image3d_max_width_;
                    ::size_t                            image3d_max_height_;
                    ::size_t                            image3d_max_depth_;
                    cl_uint                             max_samplers_;
                    ::size_t                            max_parameter_size_;
                    cl_uint                             mem_base_addr_align_;
                    cl_uint                             min_data_type_align_size_;
                    cl_device_fp_config                 single_fp_config_;
                    cl_device_mem_cache_type            global_mem_cache_type_;
                    cl_uint                             global_mem_cacheline_size_;
                    cl_ulong                            global_mem_cache_size_;
                    cl_ulong                            global_mem_size_;
                    cl_ulong                            max_constant_buffer_size_;
                    cl_uint                             max_constant_args_;
                    cl_device_local_mem_type            local_mem_type_;
                    cl_ulong                            local_mem_size_;
                    cl_bool                             error_correction_support_;
                    ::size_t                            profiling_timer_resolution_;
                    cl_bool                             endian_little_;
                    cl_bool                             available_;
                    cl_bool                             compiler_available_;
                    cl_device_exec_capabilities         execution_capabilities_;
                    cl_command_queue_properties         queue_properties_;
                    cl_platform_id                      platform_id_;
                    std::string                         name_;
                    std::string                         vendor_;
                    std::string                         driver_version_;
                    std::string                         profile_;
                    std::string                         device_version_;
                    std::string                         extensions_;

            friend  class platform;
    };

    //! \brief  This class represents the information about a specific OpenCL %platform
    //!
    //! It contains all information as provided by the driver.
    class platform
    {
        public:
                                                        platform(const platform& platform);
                    platform&                           operator=(const platform& platform);
                                                       ~platform();

                    cl_platform_id                      platform_id () const    {return platform_id_;}

                    const std::string&                  profile     () const    {return profile_;}
                    const std::string&                  version     () const    {return version_;}
                    const std::string&                  name        () const    {return name_;}
                    const std::string&                  vendor      () const    {return vendor_;}
                    const std::string&                  extensions  () const    {return extensions_;}

                    bool                                extension_supported             (const std::string& extension_name) const;

                    cl_uint                             num_devices () const;
                    device                              get_device(cl_uint index) const;

        private:
                    cl_platform_id                      platform_id_;

                    std::string                         profile_;
                    std::string                         version_;
                    std::string                         name_;
                    std::string                         vendor_;
                    std::string                         extensions_;

                    cl_uint                             num_devices_;
                    boost::scoped_array<cl_device_id>   device_ids_;

                                                        platform(cl_platform_id platform_id);

            friend  class driver;
    };

    //! \brief  This class represents the OpenCL %driver
    //!
    //! This class has no counterpart in OpenCL other than some global functions
    //! that allow querying the platforms.
    class driver
    {
        public:
                                                //! \brief Default constructor
                                                driver();
                                                //! \brief Destructor
                                               ~driver();

                cl_uint                         num_platforms() const;
                platform                        get_platform(cl_uint index) const;

        private:
                                                //! \brief The number of OpenCL platforms
            cl_uint                             num_platforms_;
                                                //! \brief An array of OpenCL %platform ID's
            boost::scoped_array<cl_platform_id> platform_ids_;
    };

    //! \brief  A function template for dumping all information retrieved from
    //!         the driver to std::cout.
    template <typename OStreamT>
    OStreamT& operator<<(OStreamT& s, const driver& drv)
    {
        s << drv.num_platforms() << " OpenCL Platform" << (drv.num_platforms() != 1 ? "s" : "") << " detected" << std::endl;

        //  For each platform
        for (cl_uint p = 0; p < drv.num_platforms(); ++p)
        {
            //  Print out platform info
            cl::platform clplatform(drv.get_platform(p));

            s << "    Platform [" << p << "]" << std::endl;
            s << "        id         = " << clplatform.platform_id   () << std::endl;
            s << "        profile    = " << clplatform.profile       () << std::endl;
            s << "        version    = " << clplatform.version       () << std::endl;
            s << "        name       = " << clplatform.name          () << std::endl;
            s << "        vendor     = " << clplatform.vendor        () << std::endl;
            s << "        extensions = ";
            {
                const std::string& ext = clplatform.extensions();

                size_t offset = 0;

                if (ext.empty() == false)
                {
                    //  Print first extension
                    size_t next_offset = ext.find(' ', offset);

                    s << ext.substr(offset, next_offset - offset) << std::endl;

                    while (next_offset != std::string::npos)
                    {
                        offset = ext.find_first_not_of(' ', next_offset);
                        next_offset = ext.find(' ', offset);

                        s << "                     " << ext.substr(offset, next_offset - offset) << std::endl;
                    }
                }
            }
            s << std::endl;
            s << "        " << clplatform.num_devices() << " Device" << (clplatform.num_devices() != 1 ? "s" : "") << " detected" << std::endl;

            for (cl_uint d = 0; d < clplatform.num_devices(); ++d)
            {
                cl::device cldevice(clplatform.get_device(d));

                s << "            Device [" << d << "]" << std::endl;
                s << "                id                            = " << cldevice.device_id                            ()  << std::endl;
                s << "                type                          = ";
                {
                    cl_device_type type = cldevice.device_type();

                    if (type & CL_DEVICE_TYPE_DEFAULT       ) s << "CL_DEVICE_TYPE_DEFAULT "    ;
                    if (type & CL_DEVICE_TYPE_CPU           ) s << "CL_DEVICE_TYPE_CPU "        ;
                    if (type & CL_DEVICE_TYPE_GPU           ) s << "CL_DEVICE_TYPE_GPU "        ;
                    if (type & CL_DEVICE_TYPE_ACCELERATOR   ) s << "CL_DEVICE_TYPE_ACCELERATOR ";

                    s << std::endl;
                }
                s << "                name                          = " << cldevice.name                                 ()  << std::endl;
                s << "                vendor                        = " << cldevice.vendor                               ()  << std::endl;
                s << "                driver version                = " << cldevice.driver_version                       ()  << std::endl;
                s << "                device version                = " << cldevice.device_version                       ()  << std::endl;
                s << "                profile                       = " << cldevice.profile                              ()  << std::endl;
                s << "                max compute units             = " << cldevice.max_compute_units                    ()  << std::endl;
                s << "                max work items dimensions     = " << cldevice.max_work_item_dimensions             () << std::endl;
                s << "                max work item sizes           = ";
                {
                    cl_uint dim = 0;
                    for (;dim < cldevice.max_work_item_dimensions() - 1; ++dim)
                    {
                        s << cldevice.max_work_item_sizes()[dim] << " / ";
                    }

                    s << cldevice.max_work_item_sizes()[dim] << std::endl;
                }
                s << "                max work group size           = " << cldevice.max_work_group_size                  () << std::endl;
                s << "                max clock frequency           = " << cldevice.max_clock_frequency()         << " MHz" << std::endl;
                s << "                address_bits                  = " << cldevice.address_bits                         () << std::endl;
                s << "                max mem alloc size            = " << (cldevice.max_mem_alloc_size() / (1024 * 1024)) << " MB" << std::endl;
                s << "                global mem size               = " << (cldevice.global_mem_size() / (1024 * 1024)) << " MB" << std::endl;
                s << "                image support                 = " << (cldevice.image_support() == CL_TRUE ? "CL_TRUE" : "CL_FALSE") << std::endl;
                s << "                max read image args           = " << cldevice.max_read_image_args                  () << std::endl;
                s << "                max write image args          = " << cldevice.max_write_image_args                 () << std::endl;
                s << "                2D image max size             = " << cldevice.image2d_max_width() << " x " << cldevice.image2d_max_height() << std::endl;
                s << "                3D image max size             = " << cldevice.image3d_max_width() << " x " << cldevice.image3d_max_height() << " x " << cldevice.image3d_max_depth() << std::endl;
                s << "                max samplers                  = " << cldevice.max_samplers                         () << std::endl;
                s << "                max parameter size            = " << cldevice.max_parameter_size                   () << std::endl;
                s << "                mem base addr align           = " << cldevice.mem_base_addr_align                  () << std::endl;
                s << "                min data type align size      = " << cldevice.min_data_type_align_size             () << std::endl;
                s << "                single fp config              = ";
                {
                    cl_device_fp_config cfg = cldevice.single_fp_config();

                    if (cfg & CL_FP_DENORM              ) s << "CL_FP_DENORM "          ;
                    if (cfg & CL_FP_INF_NAN             ) s << "CL_FP_INF_NAN "         ;
                    if (cfg & CL_FP_ROUND_TO_NEAREST    ) s << "CL_FP_ROUND_TO_NEAREST ";
                    if (cfg & CL_FP_ROUND_TO_ZERO       ) s << "CL_FP_ROUND_TO_ZERO "   ;
                    if (cfg & CL_FP_ROUND_TO_INF        ) s << "CL_FP_ROUND_TO_INF "    ;
                    if (cfg & CL_FP_FMA                 ) s << "CL_FP_FMA "             ;
                    s << std::endl;
                }
                s << "                global mem cache type         = ";
                {
                    cl_device_mem_cache_type type = cldevice.global_mem_cache_type();

                    if (type == CL_NONE             ) s << "CL_NONE"            ;
                    if (type == CL_READ_ONLY_CACHE  ) s << "CL_READ_ONLY_CACHE" ;
                    if (type == CL_READ_WRITE_CACHE ) s << "CL_READ_WRITE_CACHE";

                    s << std::endl;

                    if (type != CL_NONE)
                    {
                        s << "                global mem cacheline size     = " << cldevice.global_mem_cacheline_size            () << std::endl;
                        s << "                global mem cache size         = " << cldevice.global_mem_cache_size                () << std::endl;
                    }
                }
                s << "                max constant buffer size      = " << (cldevice.max_constant_buffer_size() / 1024) << " KB" << std::endl;
                s << "                max constant args             = " << cldevice.max_constant_args                    () << std::endl;
                s << "                local mem type                = ";
                {
                    cl_device_local_mem_type type = cldevice.local_mem_type();

                    if (type == CL_LOCAL    ) s << "CL_LOCAL";
                    if (type == CL_GLOBAL   ) s << "CL_GLOBAL";
                    s << std::endl;
                }
                s << "                local mem size                = " << (cldevice.local_mem_size() / 1024) << " KB" << std::endl;
                s << "                error correction support      = " << (cldevice.error_correction_support() == CL_TRUE ? "CL_TRUE" : "CL_FALSE") << std::endl;
                s << "                profiling timer resolution    = " << cldevice.profiling_timer_resolution() << " ns" << std::endl;
                s << "                endian little                 = " << (cldevice.endian_little() == CL_TRUE ? "CL_TRUE" : "CL_FALSE") << std::endl;
                s << "                available                     = " << (cldevice.available() == CL_TRUE ? "CL_TRUE" : "CL_FALSE") << std::endl;
                s << "                compiler available            = " << (cldevice.compiler_available() == CL_TRUE ? "CL_TRUE" : "CL_FALSE") << std::endl;
                s << "                execution capabilities        = ";
                {
                    cl_device_exec_capabilities cap = cldevice.execution_capabilities();

                    if (cap & CL_EXEC_KERNEL        ) s << "CL_EXEC_KERNEL ";
                    if (cap & CL_EXEC_NATIVE_KERNEL ) s << "CL_EXEC_NATIVE_KERNEL ";
                    s << std::endl;
                }
                s << "                queue properties              = ";
                {
                    cl_command_queue_properties prop = cldevice.queue_properties();

                    if (prop & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE   ) s << "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE ";
                    if (prop & CL_QUEUE_PROFILING_ENABLE                ) s << "CL_QUEUE_PROFILING_ENABLE "             ;
                    s << std::endl;
                }
                s << "                extensions                    = ";
                {
                    const std::string& ext = cldevice.extensions();

                    size_t offset = 0;

                    if (ext.empty() == false)
                    {
                        //  Print first extension
                        size_t next_offset = ext.find(' ', offset);

                        s << ext.substr(offset, next_offset - offset) << std::endl;

                        while (next_offset != std::string::npos)
                        {
                            offset = ext.find_first_not_of(' ', next_offset);
                            next_offset = ext.find(' ', offset);

                            s << "                                                " << ext.substr(offset, next_offset - offset) << std::endl;
                        }
                    }
                }
            }
        }

        s << std::endl;

        return s;
    }
}

#endif  //  CLCC_CLINFO_HPP_INCLUDED
