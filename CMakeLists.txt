cmake_minimum_required(VERSION 2.6)

if (NOT CMAKE_BUILD_TYPE)
    message("Setting build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

include_directories(.)

add_library( OpenCLHelper SHARED OpenCLHelper.cpp CLKernel.cpp thirdparty/clew/src/clew.c )
target_link_libraries( OpenCLHelper dl )

add_executable( gpuinfo gpuinfo.cpp thirdparty/clew/src/clew.c )
target_link_libraries( gpuinfo dl )

add_library( gtest SHARED thirdparty/gtest/gtest-all.cc )
target_link_libraries( gtest pthread)

add_executable( unittests test/testscalars.cpp test/testintarray.cpp test/testfloatwrapper.cpp
    test/testclarray.cpp test/testfloatwrapperconst.cpp test/testintwrapper.cpp test/test_scenario_te42kyfo.cpp
    test/testfloatarray.cpp test/testopenclhelper.cpp test/testinout.cpp test/testintwrapper_huge.cpp
    test/testlocal.cpp ../test/testdefines.cpp test/testbuildlog.cpp
    test/asserts.cpp thirdparty/gtest/gtest_main.cc )
target_link_libraries( unittests gtest OpenCLHelper )
target_include_directories( unittests PRIVATE thirdparty/gtest )

add_custom_target(
    git-submodule-update
    git submodule update --init --recursive
)
add_dependencies( OpenCLHelper git-submodule-update )


