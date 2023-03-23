
set( _EBS_DEPENDENCIES_TARGET_LIST
        "freetype"
        "glew"
        "lua"
        "png"
        "zlib"
        )

foreach( target ${_EBS_DEPENDENCIES_TARGET_LIST} )
    add_subdirectory( "${target}" )
    set_property( TARGET "src-dep-${target}" PROPERTY C_STANDARD 11 )
    target_compile_features( "src-dep-${target}" PRIVATE c_std_11 )
endforeach()

add_subdirectory( "assimp" )

target_include_directories( assimp PUBLIC "${ASSIMP_INCLUDE_DIRS}/assimp/include" )

target_include_directories( src-dep-png PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/zlib" )
