//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

//! \file clinfo.cpp
//! \brief OpenCL information classes source

#include "clinfo.hpp"

#include "clpp.hpp"

namespace cl
{
    namespace
    {
        //! \brief  Retrieve a single value of device info
        template <typename T>
        void
        get_device_info(cl_device_id device_id, cl_device_info device_info, T* value)
        {
            CLCALL(clGetDeviceInfo(device_id, device_info, sizeof(T), value, NULL));
        }

        //! \brief  Retrieve an array of values of device info
        template <typename T>
        void
        get_device_info(cl_device_id device_id, cl_device_info device_info, T* values, ::size_t count)
        {
            CLCALL(clGetDeviceInfo(device_id, device_info, sizeof(T) * count, values, NULL));
        }

        //! \brief  Retrieve a string value of device info
        template <>
        void
        get_device_info(cl_device_id device_id, cl_device_info device_info, std::string* value)
        {
            ::size_t size = 0;
            boost::scoped_array<char> data;

            //  Get all params for the given platform id, first query their size, then get the actual data
            CLCALL(clGetDeviceInfo(device_id, device_info, 0, NULL, &size));
            data.reset(new char[size]);
            CLCALL(clGetDeviceInfo(device_id, device_info, size, data.get(), NULL));
            value->assign(data.get(), data.get() + size);
        }
    }

    
    //! \brief  Constructor
    //! \param  device_id a valid OpenCL device ID
    //!
    //! Retrieves all available information on the given device
    device::device(cl_device_id device_id)
    :   device_id_(device_id)
    {
        //  Retrieve all device info
        get_device_info(device_id, CL_DEVICE_TYPE                           ,   &device_type_                   );
        get_device_info(device_id, CL_DEVICE_VENDOR_ID                      ,   &vendor_id_                     );
        get_device_info(device_id, CL_DEVICE_MAX_COMPUTE_UNITS              ,   &max_compute_units_             );
        get_device_info(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS       ,   &max_work_item_dimensions_      );

        //  Allocate max_work_group_size_
        max_work_item_sizes_.reset(new ::size_t[max_work_item_dimensions_]);

        get_device_info(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES            ,   max_work_item_sizes_.get()      ,   max_work_item_dimensions_);

        get_device_info(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE            ,   &max_work_group_size_           );
        get_device_info(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR    ,   &preferred_vector_width_char_   );
        get_device_info(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT   ,   &preferred_vector_width_short_  );
        get_device_info(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT     ,   &preferred_vector_width_int_    );
        get_device_info(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG    ,   &preferred_vector_width_long_   );
        get_device_info(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT   ,   &preferred_vector_width_float_  );
        get_device_info(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE  ,   &preferred_vector_width_double_ );
        get_device_info(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY            ,   &max_clock_frequency_           );
        get_device_info(device_id, CL_DEVICE_ADDRESS_BITS                   ,   &address_bits_                  );
        get_device_info(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE             ,   &max_mem_alloc_size_            );
        get_device_info(device_id, CL_DEVICE_IMAGE_SUPPORT                  ,   &image_support_                 );
        get_device_info(device_id, CL_DEVICE_MAX_READ_IMAGE_ARGS            ,   &max_read_image_args_           );
        get_device_info(device_id, CL_DEVICE_MAX_WRITE_IMAGE_ARGS           ,   &max_write_image_args_          );
        get_device_info(device_id, CL_DEVICE_IMAGE2D_MAX_WIDTH              ,   &image2d_max_width_             );
        get_device_info(device_id, CL_DEVICE_IMAGE2D_MAX_HEIGHT             ,   &image2d_max_height_            );
        get_device_info(device_id, CL_DEVICE_IMAGE3D_MAX_WIDTH              ,   &image3d_max_width_             );
        get_device_info(device_id, CL_DEVICE_IMAGE3D_MAX_HEIGHT             ,   &image3d_max_height_            );
        get_device_info(device_id, CL_DEVICE_IMAGE3D_MAX_DEPTH              ,   &image3d_max_depth_             );
        get_device_info(device_id, CL_DEVICE_MAX_SAMPLERS                   ,   &max_samplers_                  );
        get_device_info(device_id, CL_DEVICE_MAX_PARAMETER_SIZE             ,   &max_parameter_size_            );
        get_device_info(device_id, CL_DEVICE_MEM_BASE_ADDR_ALIGN            ,   &mem_base_addr_align_           );
        get_device_info(device_id, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE       ,   &min_data_type_align_size_      );
        get_device_info(device_id, CL_DEVICE_SINGLE_FP_CONFIG               ,   &single_fp_config_              );
        get_device_info(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE          ,   &global_mem_cache_type_         );
        get_device_info(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE      ,   &global_mem_cacheline_size_     );
        get_device_info(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE          ,   &global_mem_cache_size_         );
        get_device_info(device_id, CL_DEVICE_GLOBAL_MEM_SIZE                ,   &global_mem_size_               );
        get_device_info(device_id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE       ,   &max_constant_buffer_size_      );
        get_device_info(device_id, CL_DEVICE_MAX_CONSTANT_ARGS              ,   &max_constant_args_             );
        get_device_info(device_id, CL_DEVICE_LOCAL_MEM_TYPE                 ,   &local_mem_type_                );
        get_device_info(device_id, CL_DEVICE_LOCAL_MEM_SIZE                 ,   &local_mem_size_                );
        get_device_info(device_id, CL_DEVICE_ERROR_CORRECTION_SUPPORT       ,   &error_correction_support_      );
        get_device_info(device_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION     ,   &profiling_timer_resolution_    );
        get_device_info(device_id, CL_DEVICE_ENDIAN_LITTLE                  ,   &endian_little_                 );
        get_device_info(device_id, CL_DEVICE_AVAILABLE                      ,   &available_                     );
        get_device_info(device_id, CL_DEVICE_COMPILER_AVAILABLE             ,   &compiler_available_            );
        get_device_info(device_id, CL_DEVICE_EXECUTION_CAPABILITIES         ,   &execution_capabilities_        );
        get_device_info(device_id, CL_DEVICE_QUEUE_PROPERTIES               ,   &queue_properties_              );
        get_device_info(device_id, CL_DEVICE_PLATFORM                       ,   &platform_id_                   );
        get_device_info(device_id, CL_DEVICE_NAME                           ,   &name_                          );
        get_device_info(device_id, CL_DEVICE_VENDOR                         ,   &vendor_                        );
        get_device_info(device_id, CL_DRIVER_VERSION                        ,   &driver_version_                );
        get_device_info(device_id, CL_DEVICE_PROFILE                        ,   &profile_                       );
        get_device_info(device_id, CL_DEVICE_VERSION                        ,   &device_version_                );
        get_device_info(device_id, CL_DEVICE_EXTENSIONS                     ,   &extensions_                    );
    }

    //! \brief  Destructor
    device::~device()
    {
    }

    //! \brief  Checks if an extension is supported
    //! \param  extension_name the name of the extension
    //! \return \e true if the extension is supported, \e false otherwise
    bool
    device::extension_supported(const std::string& extension_name) const
    {
        return (extensions_.find(extension_name) != std::string::npos);
    }

    //! \brief Platform copy-constructor
    platform::platform(const platform& platform)
    :   platform_id_    (platform.platform_id_)
    ,   profile_        (platform.profile_)
    ,   version_        (platform.version_)
    ,   name_           (platform.name_   )
    ,   vendor_         (platform.vendor_ )
    ,   extensions_     (platform.extensions_)
    {
    }

    //! \brief Platform assignment-operator
    platform&
    platform::operator=(const platform& platform)
    {
        platform_id_    =   platform.platform_id_   ;
        profile_        =   platform.profile_       ;
        version_        =   platform.version_       ;
        name_           =   platform.name_          ;
        vendor_         =   platform.vendor_        ;
        extensions_     =   platform.extensions_    ;

        return *this;
    }

    //! \brief Platform destructor
    platform::~platform()
    {
    }

    //! \brief  Checks if an extension is supported
    //! \param  extension_name the name of the extension
    //! \return \e true if the extension is supported, \e false otherwise
    bool
    platform::extension_supported(const std::string& extension_name) const
    {
        return (extensions_.find(extension_name) != std::string::npos);
    }

    //! \brief  Returns the number of devices
    //! \return the number of devices in this platform
    cl_uint
    platform::num_devices() const
    {
        return num_devices_;
    }

    //! \brief  Returns a specific device
    //! \param index zero-based index of the device requested
    //! \return an object representing the device
    device                        
    platform::get_device(cl_uint index) const
    {
        if (index >= num_devices_)
        {
            BOOST_THROW_EXCEPTION(cl::exception("index out of range"));
        }

        return device(device_ids_[index]);
    }

    //  anonymous namespace containing some utility functions for retrieving
    //  platform info of various types
    namespace
    {
        //! \brief  Retrieve a string value of platform info
        void    
        get_platform_info(cl_platform_id platform_id, cl_platform_info platform_info, std::string* value)
        {
            ::size_t size = 0;
            boost::scoped_array<char> data;

            //  Get all params for the given platform id, first query their size, then get the actual data
            CLCALL(clGetPlatformInfo(platform_id, platform_info, 0, NULL, &size));
            data.reset(new char[size]);
            CLCALL(clGetPlatformInfo(platform_id, platform_info, size, data.get(), NULL));
            value->assign(data.get(), data.get() + size);
        }
    }

    //! \brief  Platform constructor
    //! \param  platform_id valid OpenCL %platform ID
    //!
    //! Retrieves all available information on the given platform
    platform::platform(cl_platform_id platform_id)
    :   platform_id_(platform_id)
    ,   num_devices_(0)
    {
        get_platform_info(platform_id, CL_PLATFORM_PROFILE      ,   &profile_       );
        get_platform_info(platform_id, CL_PLATFORM_VERSION      ,   &version_       );
        get_platform_info(platform_id, CL_PLATFORM_NAME         ,   &name_          );
        get_platform_info(platform_id, CL_PLATFORM_VENDOR       ,   &vendor_        );
        get_platform_info(platform_id, CL_PLATFORM_EXTENSIONS   ,   &extensions_    );

        //  Retrieve device id count
        CLCALL(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices_));

        //  Allocate
        device_ids_.reset(new cl_device_id[num_devices_]);

        //  Retrieve all id's
        CLCALL(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, num_devices_, device_ids_.get(), NULL));
    }

    //! \brief Default constructor
    driver::driver()
    :   num_platforms_(0)
    {
        //  Get number of platforms
        CLCALL(clGetPlatformIDs(0, NULL, &num_platforms_));

        //  Allocate platform id's
        platform_ids_.reset(new cl_platform_id[num_platforms_]);

        //  Retrieve platform id's
        CLCALL(clGetPlatformIDs(num_platforms_, platform_ids_.get(), NULL));
    }

    //! \brief Destructor
    driver::~driver()
    {
    }

    //! \brief Retrieve the number of platforms
    //! \return the number of OpenCL platforms
    cl_uint
    driver::num_platforms() const
    {
        return num_platforms_;
    }

    //! \brief  Returns a specific platform
    //! \param index zero-based index of the platform requested
    //! \return an object representing the platform
    platform
    driver::get_platform(cl_uint index) const
    {
        if (index >= num_platforms_)
        {
            BOOST_THROW_EXCEPTION(cl::exception("index out of range"));
        }

        return platform(platform_ids_[index]);
    }
}   //  namespace cl
