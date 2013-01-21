#ifndef CLCC_CLEXCEPTION_HPP_INCLUDED
#define CLCC_CLEXCEPTION_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2009-2011 Organic Vectory B.V.
//  Written by George van Venrooij
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file license.txt)
//////////////////////////////////////////////////////////////////////////

//! \file clexception.hpp
//! \brief C++ exception class for potential errors
//!
//! This file contains an exception class definition that is to be used 
//! under possible error conditions that can be encountered when running
//! CLCC.
//!
//! The intention of this class is to provide custom handling when these
//! specific errors occur.

#include <boost/exception/all.hpp>

#include <iostream>

namespace cl
{
    //! \brief  Exception class derived from both std::exception and boost::exception
    struct exception : virtual std::exception, virtual boost::exception
    {
        exception(const std::string& message)
            :   message_(message)
        {
            std::cerr << "error: " << message << std::endl;
        }

#ifdef  __GNUC__
        ~exception() throw() {}
#endif

        private:    
            std::string message_;
    };
}

#endif  //  CLCC_CLEXCEPTION_HPP_INCLUDED
