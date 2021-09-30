
set( TS3SAMPLES_LINK_INPUT
        ts3-system
        #ts3-engine
        #ts3-gpuapi
        #ts3drv-gpuapi-gl4
        #ts3drv-gpuapi-gles3
        #ts3drv-gpuapi-dx11
        #ts3ext-rcsupport
)

set( TS3_SAMPLE_LIST
        #"mathTest"
        #"test1"
        "sysTest" )

foreach( module ${TS3_SAMPLE_LIST} )
    add_subdirectory( "${module}" )
endforeach()
