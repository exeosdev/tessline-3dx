
set( TS3_SCRIPT_SRC_FILES
    "prerequisites.h"
    "prerequisites.cpp"
)

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3-script STATIC ${TS3_SCRIPT_SRC_FILES} )
    target_compile_definitions( ts3-script PRIVATE
			"TS3_BUILD_STATIC=1")
else()
    add_library( ts3-script SHARED ${TS3_SCRIPT_SRC_FILES} )
endif()

set_target_properties( ts3-script PROPERTIES
		OUTPUT_NAME "ts3dx-component-script"
		)

target_compile_definitions( ts3-script PRIVATE
		"TS3_SCRIPT_BUILD=1"
		)

target_include_directories( ts3-gpuapi PUBLIC
		"${TS3_INCLUDE_DIR}"
		)

target_link_libraries( ts3-script PUBLIC
		)
