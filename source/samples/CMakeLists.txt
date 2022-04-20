
set( TS3SAMPLES_LINK_INPUT
        ts3-system
        )

if( TARGET ts3drv-gpuapi-dx11 )
    #set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-dx11" )
endif()

if( TARGET ts3drv-gpuapi-dx12 )
    #set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-dx12" )
endif()

if( TARGET ts3drv-gpuapi-gl4 )
    #set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-gl4" )
endif()

if( TARGET ts3drv-gpuapi-gles3 )
    #set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-gles3" )
endif()

set( TS3_SAMPLE_LIST
        #"mathTest"
        "sysTest"
        "test1"
        )

foreach( module ${TS3_SAMPLE_LIST} )
    add_subdirectory( "${module}" )
endforeach()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "macOS" )
    add_subdirectory( "appleCocoaTest" )
endif()
