
set( TS3SAMPLES_LINK_INPUT
        ts3-system
        )

set( TS3_SAMPLE_LIST
        #"mathTest"
        "gpuapi01"
        "sysTest"
        "test1"
        )

foreach( module ${TS3_SAMPLE_LIST} )
    add_subdirectory( "${module}" )
endforeach()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "macOS" )
    add_subdirectory( "appleCocoaTest" )
endif()
