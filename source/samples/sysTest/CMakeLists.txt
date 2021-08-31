
add_executable( ts3sample-sysTest
        "sysTest-main.cpp" )

set_target_properties( ts3sample-sysTest PROPERTIES
        OUTPUT_NAME "ts3sample-sysTest" )

target_link_libraries( ts3sample-sysTest PUBLIC
        ${TS3SAMPLES_LINK_INPUT} )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-sysTest PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()