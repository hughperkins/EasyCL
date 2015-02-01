cmake_minimum_required(VERSION 2.6)

if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
  set( ON_LINUX 1 )
endif()

if (NOT CMAKE_BUILD_TYPE)
    message("Setting build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

include_directories(.)

add_library( CLEW thirdparty/clew/src/clew.c )
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES( CLEW PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

add_library( OpenCLHelper SHARED OpenCLHelper.cpp CLKernel.cpp )
target_link_libraries( OpenCLHelper CLEW )
if( ON_LINUX )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
target_link_libraries( OpenCLHelper dl )
endif()

add_executable( gpuinfo gpuinfo.cpp )
target_link_libraries( gpuinfo CLEW )
if( ON_LINUX )
target_link_libraries( gpuinfo dl )
endif()

if( ON_LINUX )
add_library( gtest SHARED thirdparty/gtest/gtest-all.cc )
target_link_libraries( gtest pthread)
else()
add_library( gtest thirdparty/gtest/gtest-all.cc )
endif()

add_executable( unittests test/testscalars.cpp test/testintarray.cpp test/testfloatwrapper.cpp
    test/testclarray.cpp test/testfloatwrapperconst.cpp test/testintwrapper.cpp test/test_scenario_te42kyfo.cpp
    test/testfloatarray.cpp test/testopenclhelper.cpp test/testinout.cpp test/testintwrapper_huge.cpp
    test/testlocal.cpp ../test/testdefines.cpp test/testbuildlog.cpp test/testnewinstantiations.cpp
    test/asserts.cpp thirdparty/gtest/gtest_main.cc )
target_link_libraries( unittests gtest OpenCLHelper CLEW )
target_include_directories( unittests PRIVATE thirdparty/gtest )

execute_process( COMMAND gitx --version RESULT_VARIABLE git_test ERROR_QUIET )
if( ${git_test} EQUAL 0 )
    set( GIT_FOUND 1 )
endif()
if( GIT_FOUND )
message("git found")
endif()

if( GIT_FOUND )
add_custom_target(
    git-submodule-update
    git submodule update --init --recursive
)
add_dependencies( OpenCLHelper git-submodule-update )
endif()

if( ON_LINUX )
add_custom_target( 
    clfiles
    cp ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cl ${CMAKE_BINARY_DIR}
)
add_dependencies( unittests clfiles )
else()
# No idea how to do this on visual studio/windows
# If you have any ideas :-)
#add_custom_target( 
#    clfiles
#    copy ${CMAKE_CURRENT_SOURCE_DIR}\\tests\\*.cl ${CMAKE_BINARY_DIR}\\Release
#)
endif()

