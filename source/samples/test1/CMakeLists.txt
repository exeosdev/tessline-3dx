
add_executable( ts3sample-test1
        "test1-meshDefs.h"
        "test1-main.cpp" )

set_target_properties( ts3sample-test1 PROPERTIES
        OUTPUT_NAME "ts3sample-test1" )

target_link_libraries( ts3sample-test1 PUBLIC
        ${TS3SAMPLES_LINK_INPUT} )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-test1 PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()