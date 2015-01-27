#pragma once

#if defined(_WIN32) 
# if defined(OpenCLHelper_EXPORTS)
#  define OpenCLHelper_EXPORT __declspec(dllexport)
# else
#  define OpenCLHelper_EXPORT __declspec(dllimport)
# endif 
#else 
# define OpenCLHelper_EXPORT
#endif

