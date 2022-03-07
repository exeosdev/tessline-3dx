
if( "${CM_EBS_SYSCONF_TARGET_PLATFORM}" STREQUAL "Android" )
    add_library( ts3sample-test1 STATIC
            "test1-main.cpp" )
else()
    add_executable( ts3sample-test1
            "test1-meshDefs.h"
            "test1-main.cpp" )
endif()

set_target_properties( ts3sample-test1 PROPERTIES
        OUTPUT_NAME "ts3sample-test1" )

target_link_libraries( ts3sample-test1 PUBLIC
        ts3-platform
        ts3-core
        ts3-engine
        ts3-system )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-test1 PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()
