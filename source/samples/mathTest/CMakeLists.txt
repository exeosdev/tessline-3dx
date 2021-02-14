
add_executable( ts3sample-mathTest
        "mathTest-main.cpp" )

set_target_properties( ts3sample-mathTest PROPERTIES
        OUTPUT_NAME "ts3sample-mathTest" )

target_link_libraries( ts3sample-mathTest PUBLIC
        exf-module-math )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-mathTest PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()