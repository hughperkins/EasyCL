cmake_minimum_required(VERSION 2.6)

if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
    set( ON_LINUX 1 )
endif()

if( ON_LINUX )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if (NOT CMAKE_BUILD_TYPE)
#    message("Setting build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

OPTION(BUILD_TESTS "Build unit-tests." OFF)
OPTION(DEV_RUN_COG "Only EasyCL maintainers need this, otherwise set to 'OFF'." OFF)
OPTION(PROVIDE_LUA_ENGINE "eg for Torch, we use Torch's engine." ON)
#set(TYPEDEF_INT64 int64_t CACHE STRING "typedef for int64")
#set(TYPEDEF_UINT64 uint64_t CACHE STRING "typedef for uint64")

add_definitions(-DTYPEDEF_INT64=${TYPEDEF_INT64})
add_definitions(-DTYPEDEF_UINT64=${TYPEDEF_UINT64})

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/dist" CACHE PATH "Installation prefix, default 'dist'" FORCE
    )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if(BUILD_TESTS AND NOT PROVIDE_LUA_ENGINE)
  # find_library(Lua51 REQUIRED)
  message("Error: if building tests, need to enable PROVIDE_LUA_ENGINE")
endif()

# remove obsolete options, so dont clutter the ccmake config page
unset(INT64_TYPEDEF CACHE)
unset(UINT64_TYPEDEF CACHE)
unset(TYPEDEF_INT64 CACHE)
unset(TYPEDEF_UINT64 CACHE)
unset(LUA_AVAILABLE CACHE)
unset(KERNEL_TEMPLATING CACHE)
unset(CLBLAS_DIR CACHE)
unset(TEST_CLBLAS CACHE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/clew/include)
# include_directories("${CMAKE_INSTALL_PREFIX}/include")

include_directories(thirdparty/lua-5.1.5/src)
set(TEMPLATESRC templates/LuaTemplater.cpp templates/TemplatedKernel.cpp )
set(TEMPLATETESTS test/testLuaTemplater.cpp test/testTemplatedKernel.cpp )

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

INCLUDE("${CMAKE_MODULE_PATH}/build_clew.cmake")
INCLUDE_DIRECTORIES(${CLEW_INCLUDE_DIRS})

if(PROVIDE_LUA_ENGINE)
    file(STRINGS thirdparty/lua-5.1.5/files.txt lua_src1)
    foreach(source ${lua_src1})
        set( lua_src ${lua_src} thirdparty/lua-5.1.5/src/${source})
    endforeach()
endif(PROVIDE_LUA_ENGINE)

#add_definitions(-DCL_USE_DEPRECATED_OPENCL_1_0_APIS)

add_library( EasyCL SHARED EasyCL.cpp CLKernel.cpp CLWrapper.cpp platforminfo_helper.cpp deviceinfo_helper.cpp DevicesInfo.cpp DeviceInfo.cpp 
  util/easycl_stringhelper.cpp util/StatefulTimer.cpp
  ${lua_src}
  ${TEMPLATESRC} )
if(WIN32)
    target_link_libraries(EasyCL winmm) # needed for timeGetTime
endif()
target_link_libraries( EasyCL ${CLEW_LIBRARIES} )
if( ON_LINUX )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    target_link_libraries( EasyCL dl )
endif()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

if(NOT PROVIDE_LUA_ENGINE)
  IF(APPLE)
    SET_TARGET_PROPERTIES(EasyCL PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  ENDIF(APPLE)
else()
  IF(APPLE)
    SET_TARGET_PROPERTIES(EasyCL PROPERTIES LINK_FLAGS "-single_module")
    ADD_DEFINITIONS(-DLUA_USE_LINUX)
  ENDIF(APPLE)
endif(NOT PROVIDE_LUA_ENGINE)

add_executable( gpuinfo gpuinfo.cpp platforminfo_helper.cpp deviceinfo_helper.cpp DeviceInfo.cpp DeviceInfo.cpp )
target_link_libraries( gpuinfo ${CLEW_LIBRARIES} )
if( ON_LINUX )
    target_link_libraries( gpuinfo dl )
endif()

if(BUILD_TESTS)
  if( ON_LINUX )
      add_library( easycl_gtest SHARED thirdparty/gtest/gtest-all.cc )
      target_link_libraries( easycl_gtest pthread)
  else()
      add_library( easycl_gtest thirdparty/gtest/gtest-all.cc )
  endif()

  add_executable( easycl_unittests test/testscalars.cpp test/testintarray.cpp test/testfloatwrapper.cpp
      test/testclarray.cpp test/testfloatwrapperconst.cpp test/testintwrapper.cpp test/test_scenario_te42kyfo.cpp
      test/testfloatarray.cpp test/testeasycl.cpp test/testinout.cpp test/testintwrapper_huge.cpp
      test/testlocal.cpp test/testdefines.cpp test/testbuildlog.cpp test/testnewinstantiations.cpp
      test/testucharwrapper.cpp test/testkernelstore.cpp test/testdirtywrapper.cpp test/testDeviceInfo.cpp
      ${TEMPLATETESTS} test/easycl_stringhelper.cpp test/testStructs.cpp
      test/asserts.cpp test/gtest_main.cpp test/GtestGlobals.cpp test/testprofiling.cpp
      test/testcopybuffer.cpp test/teststatefultimer.cpp
      ${CLBLAS_TEST_SOURCES} )
  target_link_libraries( easycl_unittests easycl_gtest EasyCL ${CLEW_LIBRARIES} )
  target_include_directories( easycl_unittests PRIVATE thirdparty/gtest )
endif(BUILD_TESTS)

if(DEV_RUN_COG)
    add_custom_target(
        cog
        python ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cogapp/cog.py -q -I ${CMAKE_CURRENT_SOURCE_DIR}/cog-batteries -r ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.h ${CMAKE_CURRENT_SOURCE_DIR}/util/*.h
    )
    add_dependencies( EasyCL cog )
endif(DEV_RUN_COG)

ADD_DEPENDENCIES(EasyCL clew)
ADD_DEPENDENCIES(gpuinfo clew)

INSTALL( TARGETS EasyCL
     RUNTIME DESTINATION bin
     ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib )
	
FILE(GLOB HEADERS_ROOT *.h )
FILE(GLOB HEADERS_TEMPLATES templates/*.h )
FILE(GLOB HEADERS_UTIL util/*.h)
INSTALL(FILES ${HEADERS_ROOT} DESTINATION include/easycl)
INSTALL(FILES ${HEADERS_TEMPLATES} DESTINATION include/easycl/templates)
INSTALL(FILES ${HEADERS_UTIL} DESTINATION include/easycl/util)
INSTALL(PROGRAMS easycl_activate.sh DESTINATION bin)
if(BUILD_TESTS)
   FILE(GLOB CLFILES test/*.cl)
   INSTALL(FILES ${CLFILES} DESTINATION bin)
   INSTALL(TARGETS easycl_gtest easycl_unittests gpuinfo
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
      RUNTIME DESTINATION bin
   )
    # Note: to run tests, need the .cl files currently, so need to install those
    # too, and reference them appropriately from the code, ie correct path
endif(BUILD_TESTS)


