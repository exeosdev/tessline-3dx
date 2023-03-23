
set( TS3_EXTENSIONS_INCLUDE_DIR_INTERNAL
        ${TS3_INCLUDE_DIR}
        "${TS3_BASE_DIR}/source/engine/extensions" )

set( _EBS_ENGINE_EXTENSIONS_TARGET_LIST
        "rcsupport"
        )

foreach( target ${_EBS_ENGINE_EXTENSIONS_TARGET_LIST} )
    add_subdirectory( "ts3ext/${target}" )
    if( TARGET "ts3ext-${target}" )
        set_property( TARGET "ts3ext-${target}" PROPERTY CXX_STANDARD 17 )
        target_compile_features( "ts3ext-${target}" PRIVATE cxx_std_17 )
    endif()
endforeach()
