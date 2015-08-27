#pragma once

// msvc doesnt have c99 uint32_t etc, so we define them ourselves
// more info here: http://stackoverflow.com/questions/126279/c99-stdint-h-header-and-ms-visual-studio
#if _MSC_VER == 1500  // msvc 2008
    typedef __int32 int32_t;
    typedef __int64 int64_t;
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int64 uint64_t;
#else // everything else...
    #include <cstdint>
#endif

