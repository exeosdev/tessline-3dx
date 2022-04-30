
set( TS3SAMPLES_LINK_INPUT
        ts3-system
        )

set( TS3_SAMPLE_LIST
        "gpuapi01"
        "sysTest"
        "test1"
        )

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "macOS" )
    set( TS3_SAMPLE_LIST "${TS3_SAMPLE_LIST}" "cocoa" )
endif()

foreach( module ${TS3_SAMPLE_LIST} )
    add_subdirectory( "${module}" )
endforeach()
