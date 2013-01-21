#ifndef CLCC_CLPP_HPP_INCLUDED
#define CLCC_CLPP_HPP_INCLUDED

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

//! \file clpp.hpp
//! \brief C++ wrappers around OpenCL constructs
//!
//! This file contains a resource tracking class template for various OpenCL
//! reference counted resources and a macro for wrapping OpenCL error codes and
//! function calls into a generic error handler.

#include "clew.h"
#include "clexception.hpp"

namespace cl
{
    //! \brief Helper class template to handle resource tracking of OpenCL reference counted resources
    template <typename T>
    struct shared_item
    {
        //! \brief  Default constructor
        shared_item()
            :   item_()
        {
        }

        //! \brief Initializing constructor, assumes caller has retained item
        shared_item(T item)
            :   item_(item)
        {
        }

        //! \brief Assignment operator, increments reference count
        shared_item&    operator=(const shared_item& other)
        {
            destroy();

            item_ = other.item_;

            retain_item(item_);

            return *this;
        }

        //! \brief Copy constructor, increments reference count
        shared_item(const shared_item& other)
            :   item_(other.item_)
        {
            retain_item(item_);
        }

        //! \brief Destructor, decrements reference count
       ~shared_item()
        {
            destroy();
        }

        //! \brief Retrieve item value
        T   get() const
        {
            if (item_ == T())
            {
                throw std::runtime_error("Unitialized item");
            }

            return item_;
        }
    
        private:
            //! \brief  item holder
            T           item_;

            //! \brief  reference count decrement function
            void destroy()
            {
                //  We ignore an error when releasing the item, since the destructor should not throw
                if (item_ != T())
                {
                    delete_item(item_);
                }
            }

        //! \brief Function that decreases the item reference count
        static  void    delete_item(T item);
        //! \brief Function that increases the item reference count
        static  void    retain_item(T item);
    };

    //! \brief  Reference-counted \b cl_context class
    typedef shared_item<cl_context          >   shared_context;
    //! \brief  Reference-counted \b cl_mem class
    typedef shared_item<cl_mem              >   shared_buffer;
    //! \brief  Reference-counted \b cl_program class
    typedef shared_item<cl_program          >   shared_program;
    //! \brief  Reference-counted \b cl_command_queue class
    typedef shared_item<cl_command_queue    >   shared_queue;
    //! \brief  Reference-counted \b cl_kernel class
    typedef shared_item<cl_kernel           >   shared_kernel;

    //! \brief  Specialized item release function
    template <> inline  void shared_item<cl_context         >::delete_item(cl_context       item){clReleaseContext      (item);}
    //! \brief  Specialized item release function
    template <> inline  void shared_item<cl_mem             >::delete_item(cl_mem           item){clReleaseMemObject    (item);}
    //! \brief  Specialized item release function
    template <> inline  void shared_item<cl_program         >::delete_item(cl_program       item){clReleaseProgram      (item);}
    //! \brief  Specialized item release function
    template <> inline  void shared_item<cl_command_queue   >::delete_item(cl_command_queue item){clReleaseCommandQueue (item);}
    //! \brief  Specialized item release function
    template <> inline  void shared_item<cl_kernel          >::delete_item(cl_kernel        item){clReleaseKernel       (item);}

    //! \brief  Specialized item retain function
    template <> inline  void shared_item<cl_context         >::retain_item(cl_context       item){clRetainContext       (item);}
    //! \brief  Specialized item retain function
    template <> inline  void shared_item<cl_mem             >::retain_item(cl_mem           item){clRetainMemObject     (item);}
    //! \brief  Specialized item retain function
    template <> inline  void shared_item<cl_program         >::retain_item(cl_program       item){clRetainProgram       (item);}
    //! \brief  Specialized item retain function
    template <> inline  void shared_item<cl_command_queue   >::retain_item(cl_command_queue item){clRetainCommandQueue  (item);}
    //! \brief  Specialized item retain function
    template <> inline  void shared_item<cl_kernel          >::retain_item(cl_kernel        item){clRetainKernel        (item);}

}   //  namespace cl


#ifndef CLCC_GENERATE_DOCUMENTATION

#ifndef NDEBUG
//  In debug mode, make this a full function
inline void CLCALL(cl_int error)
{
    if (error != CL_SUCCESS) 
    {
        BOOST_THROW_EXCEPTION( ::cl::exception(clewErrorString(error)) );
    }
}
#else
#define CLCALL(error)   if (error != CL_SUCCESS) BOOST_THROW_EXCEPTION( ::cl::exception(clewErrorString(error)) )
#endif

#else

#define CLCALL(error)   //!<    Macro that takes an OpenCL error code and throws an exception if it holds any other value than CL_SUCCESS

#endif  //  CLCC_GENERATE_DOCUMENTATION

#endif  //  CLCC_CLPP_HPP_INCLUDED
