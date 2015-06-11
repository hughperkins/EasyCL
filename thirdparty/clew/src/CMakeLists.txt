
add_library( clewlib SHARED clew.c )

set_target_properties(clewlib
    PROPERTIES
        OUTPUT_NAME clew
        CLEAN_DIRECT_OUTPUT 1
        SOVERSION "1.0.0"
    )

target_link_libraries( clewlib dl )

install(TARGETS clewlib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
