
add_executable( ts3sample-cocoa
        "cocoaMain.mm" )

set_target_properties( ts3sample-cocoa PROPERTIES
        OUTPUT_NAME "ts3sample-cocoa" )

target_link_libraries( ts3sample-cocoa PUBLIC
        ${TS3SAMPLES_LINK_INPUT} )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-cocoa PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()
