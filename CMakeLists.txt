cmake_minimum_required(VERSION 2.6)

if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
    set( ON_LINUX 1 )
endif()

if( ON_LINUX )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if( ON_LINUX )
    set(TEST_CLBLAS "No" CACHE BOOL "Run the clBLAS GEMM test?  clBlas should be at ../clBLAS, and built into ../clBLAS/src/build")
endif()

if (NOT CMAKE_BUILD_TYPE)
    message("Setting build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

unset( LUA_AVAILABLE CACHE)
OPTION(BUILD_TESTS "Build unit-tests." OFF)
OPTION(KERNEL_TEMPLATING "Activate kernel templating, at runtime, using Lua." ON)
OPTION(DEV_RUN_COG "Only EasyCL maintainers need this, otherwise set to 'OFF'." OFF)
SET(CLBLAS_DIR "../clMathLibraries/clBLAS" CACHE PATH "Only needed if TEST_CLBLAS is On.")

include_directories(.)

if(KERNEL_TEMPLATING)
    find_package(Lua51)
    include_directories(${LUA_INCLUDE_DIR})
    link_libraries(${LUA_LIBRARIES})
    set(TEMPLATESRC templates/LuaTemplater.cpp templates/TemplatedKernel.cpp )
    set(TEMPLATETESTS test/testLuaTemplater.cpp test/testTemplatedKernel.cpp )
endif(KERNEL_TEMPLATING)

add_library( CLEW thirdparty/clew/src/clew.c )
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
    SET_TARGET_PROPERTIES( CLEW PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

add_library( EasyCL SHARED EasyCL.cpp CLKernel.cpp CLWrapper.cpp platforminfo_helper.cpp deviceinfo_helper.cpp DevicesInfo.cpp DeviceInfo.cpp util/easycl_stringhelper.cpp
    ${TEMPLATESRC} )
target_link_libraries( EasyCL CLEW )
if( ON_LINUX )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    target_link_libraries( EasyCL dl )
endif()

add_executable( gpuinfo gpuinfo.cpp platforminfo_helper.cpp deviceinfo_helper.cpp DeviceInfo.cpp DeviceInfo.cpp )
target_link_libraries( gpuinfo CLEW )
if( ON_LINUX )
    target_link_libraries( gpuinfo dl )
endif()

if(BUILD_TESTS)
  # message("BUILD_TESTS ${BUILD_TESTS}")
  if( ON_LINUX )
      add_library( easycl_gtest SHARED thirdparty/gtest/gtest-all.cc )
      target_link_libraries( easycl_gtest pthread)
  else()
      add_library( easycl_gtest thirdparty/gtest/gtest-all.cc )
  endif()

  if( TEST_CLBLAS )
      set( CLBLAS_TEST_SOURCES test/testclblas.cpp )
  endif()

  add_executable( easycl_unittests test/testscalars.cpp test/testintarray.cpp test/testfloatwrapper.cpp
      test/testclarray.cpp test/testfloatwrapperconst.cpp test/testintwrapper.cpp test/test_scenario_te42kyfo.cpp
      test/testfloatarray.cpp test/testeasycl.cpp test/testinout.cpp test/testintwrapper_huge.cpp
      test/testlocal.cpp test/testdefines.cpp test/testbuildlog.cpp test/testnewinstantiations.cpp
      test/testucharwrapper.cpp test/testkernelstore.cpp test/testdirtywrapper.cpp test/testDeviceInfo.cpp
      ${TEMPLATETESTS} test/easycl_stringhelper.cpp test/testStructs.cpp
      test/asserts.cpp test/gtest_main.cpp test/GtestGlobals.cpp 
      test/testcopybuffer.cpp
      ${CLBLAS_TEST_SOURCES} )
  target_link_libraries( easycl_unittests easycl_gtest EasyCL CLEW )
  target_include_directories( easycl_unittests PRIVATE thirdparty/gtest )

  if( TEST_CLBLAS )
      target_include_directories( easycl_unittests PRIVATE ${CLBLAS_DIR}/src )
      target_link_libraries( easycl_unittests ${CLBLAS_DIR}/src/build/library/libclBLAS.so)
  endif()
endif(BUILD_TESTS)

if(DEV_RUN_COG)
    add_custom_target(
        cog
        python ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cogapp/cog.py -q -I ${CMAKE_CURRENT_SOURCE_DIR}/cog-batteries -r ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.h
    )
    add_dependencies( EasyCL cog )
endif(DEV_RUN_COG)

INSTALL( TARGETS EasyCL
    LIBRARY DESTINATION lib )
INSTALL( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION include/easycl FILES_MATCHING PATTERN *.h PATTERN "install" EXCLUDE PATTERN "build*" EXCLUDE 
 PATTERN "test" EXCLUDE PATTERN "cog-batteries" EXCLUDE PATTERN ".git" EXCLUDE )

if(BUILD_TESTS)
#    INSTALL(TARGETS easycl_gtest easycl_unittests
#       LIBRARY DESTINATION lib
#       RUNTIME DESTINATION bin
#    )
    # Note: to run tests, need the .cl files currently, so need to install those
    # too, and reference them appropriately from the code, ie correct path
endif(BUILD_TESTS)


