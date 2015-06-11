
add_executable( clewTest clewTest.cpp )

target_link_libraries( clewTest clewlib )

install(TARGETS clewTest DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

