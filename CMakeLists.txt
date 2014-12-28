cmake_minimum_required(VERSION 2.6)

if (NOT CMAKE_BUILD_TYPE)
    message("Setting build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# This is not used yet, but it will be soon ;-)
include_directories(.)
include_directories(clew/include)
#add_library( clew SHARED clew/src/clew.c )
#target_link_libraries( clew dl )

add_library( OpenCLHelper SHARED OpenCLHelper.cpp CLKernel.cpp clew/src/clew.c )
target_link_libraries( OpenCLHelper dl )

add_executable( gpuinfo gpuinfo.cpp )
target_link_libraries( gpuinfo OpenCLHelper )

add_executable( testopenclhelper test/testopenclhelper.cpp )
target_link_libraries( testopenclhelper OpenCLHelper )

add_executable( testinout test/testinout.cpp )
target_link_libraries( testinout OpenCLHelper )

add_executable( testclarray test/testclarray.cpp )
target_link_libraries( testclarray OpenCLHelper )

add_executable( testintarray test/testintarray.cpp )
target_link_libraries( testintarray OpenCLHelper )

add_executable( testfloatarray test/testfloatarray.cpp )
target_link_libraries( testfloatarray OpenCLHelper )

add_executable( testintwrapper test/testintwrapper.cpp )
target_link_libraries( testintwrapper OpenCLHelper )

add_executable( testfloatwrapper test/testfloatwrapper.cpp )
target_link_libraries( testfloatwrapper OpenCLHelper )

add_executable( testfloatwrapper_huge test/testfloatwrapper_huge.cpp )
target_link_libraries( testfloatwrapper_huge OpenCLHelper )

add_executable( testfloatwrapperconst test/testfloatwrapperconst.cpp )
target_link_libraries( testfloatwrapperconst OpenCLHelper )

add_executable( testscalars test/testscalars.cpp )
target_link_libraries( testscalars OpenCLHelper )

add_executable( test_scenario_te42kyfo test/test_scenario_te42kyfo.cpp )
target_link_libraries( test_scenario_te42kyfo OpenCLHelper )



