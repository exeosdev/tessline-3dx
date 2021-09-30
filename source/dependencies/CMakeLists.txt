
set( _EBS_DEPENDENCIES_TARGET_LIST
        "freetype"
        "glew"
        "lua"
        "zlib"
        )

foreach( target ${_EBS_DEPENDENCIES_TARGET_LIST} )
    add_subdirectory( "${target}" )
    set_property( TARGET "dplib-${target}" PROPERTY C_STANDARD 11)
    target_compile_features( "dplib-${target}" PRIVATE c_std_11 )
endforeach()
