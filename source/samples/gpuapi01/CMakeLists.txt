
set( gpuapi01_SRC_FILES_COMMON
        "gpuapi01-meshDefs.h"
        "gpuapi01-main.cpp"
        )

set( gpuapi01_SRC_FILES_APPLE
        "gpuapi01-meshDefs.h"
        "gpuapi01-main.mm"
        )

set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT}
        ts3-engine
        ts3-gpuapi
        )

if( TARGET ts3drv-gpuapi-dx11 )
    set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-dx11" )
endif()

if( TARGET ts3drv-gpuapi-dx12 )
    set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-dx12" )
endif()

if( TARGET ts3drv-gpuapi-gl4 )
    set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-gl4" )
endif()

if( TARGET ts3drv-gpuapi-gles3 )
    set( TS3SAMPLES_LINK_INPUT ${TS3SAMPLES_LINK_INPUT} "ts3drv-gpuapi-gles3" )
endif()

if( "${CM_EBS_SYSCONF_TARGET_PLATFORM}" STREQUAL "Apple" )
    add_executable( ts3sample-gpuapi01
            ${gpuapi01_SRC_FILES_APPLE}
            )
elseif( "${CM_EBS_SYSCONF_TARGET_PLATFORM}" STREQUAL "Android" )
    add_library( ts3sample-gpuapi01 STATIC
            ${gpuapi01_SRC_FILES_COMMON}
            )
else()
    add_executable( ts3sample-gpuapi01
            ${gpuapi01_SRC_FILES_COMMON}
            )
endif()

set_target_properties( ts3sample-gpuapi01 PROPERTIES
        OUTPUT_NAME "ts3sample-gpuapi01" )

target_link_libraries( ts3sample-gpuapi01 PUBLIC
        ${TS3SAMPLES_LINK_INPUT} )

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    target_compile_definitions( ts3sample-gpuapi01 PRIVATE
            "TS3_BUILD_STATIC=1" )
endif()
