
set( TS3_CDEP_GLEW_SCR_FILES
		glew.c )

add_library( src-dep-glew STATIC
        ${TS3_CDEP_GLEW_SCR_FILES} )

set_target_properties( src-dep-glew PROPERTIES
        OUTPUT_NAME "glew" )

target_include_directories( src-dep-glew PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
		${TS3_BASE_DIR}/external/include )

target_compile_definitions( src-dep-glew PUBLIC
		GLEW_BUILD=1
		GLEW_STATIC=1
		_LIB=1
		WIN32_LEAN_AND_MEAN )

if( "${CM_EBS_SYSCONF_HOST_COMPILER}" MATCHES "Clang|GCC" )
	target_compile_options( src-dep-glew PRIVATE
			-Wno-implicit-function-declaration
			)
endif()
