
add_executable( ts3sample-appleCocoaTest
        "cocoaMain.cpp" )

set_target_properties( ts3sample-appleCocoaTest PROPERTIES
        OUTPUT_NAME "ts3sample-appleCocoaTest" )

target_link_libraries( ts3sample-appleCocoaTest PUBLIC
        ts3-system )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-appleCocoaTest PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()
