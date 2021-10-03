
set( _EBS_ENGINE_COMPONENTS_TARGET_LIST
        "platform"
        "core"
        "engine"
        "gpuapi"
        "math"
        "script"
        "stdext"
        "system"
        )

foreach( target ${_EBS_ENGINE_COMPONENTS_TARGET_LIST} )
    add_subdirectory( "ts3/${target}" )
    if( TARGET "ts3-${target}" )
        set_property( TARGET "ts3-${target}" PROPERTY CXX_STANDARD 17 )
        target_compile_features( "ts3-${target}" PUBLIC cxx_std_17 )
    endif()
endforeach()
