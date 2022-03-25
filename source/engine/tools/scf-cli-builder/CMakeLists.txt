
add_executable( ts3tool-scf-cli-builder
        "main.cpp"
        )

set_target_properties( ts3tool-scf-cli-builder PROPERTIES
        OUTPUT_NAME "scf-cli-builder" )

target_link_libraries( ts3tool-scf-cli-builder PUBLIC
        ${TS3_TOOLS_LINK_INPUT} )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3tool-scf-cli-builder PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()
