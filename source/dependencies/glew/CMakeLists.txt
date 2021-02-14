
set( TS3_CDEP_GLEW_SCR_FILES
		glew.c )

add_library( dplib-glew STATIC
        ${TS3_CDEP_GLEW_SCR_FILES} )

set_target_properties( dplib-glew PROPERTIES
        OUTPUT_NAME "glew" )

target_include_directories( dplib-glew PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
		${TS3_BASE_DIR}/external/include )

target_compile_definitions( dplib-glew PUBLIC
		GLEW_BUILD=1
		GLEW_STATIC=1
		_LIB=1
		WIN32_LEAN_AND_MEAN )
