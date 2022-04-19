
if( "${CM_EBS_SYSCONF_TARGET_PLATFORM}" STREQUAL "Android" )
    add_library( ts3sample-sysTest STATIC
            #"sysTest-gpuapi-meshDefs.h"
            #"sysTest-gpuapi.cpp"
            "sysTest-main.cpp"
            )
else()
    add_executable( ts3sample-sysTest
            #"sysTest-gpuapi-meshDefs.h"
            #"sysTest-gpuapi.cpp"
            "sysTest-main.cpp"
            )
endif()

set_target_properties( ts3sample-sysTest PROPERTIES
        OUTPUT_NAME "ts3sample-sysTest" )

target_link_libraries( ts3sample-sysTest PUBLIC
        ${TS3SAMPLES_LINK_INPUT} )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-sysTest PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()
