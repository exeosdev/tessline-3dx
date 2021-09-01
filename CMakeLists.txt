
cmake_minimum_required( VERSION 3.10 )

set( CMAKE_C_STANDARD 11 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS OFF )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

project( tessline-3dx )

include( "cmake/CMakeLists.txt" )

if( "${CM_EBS_SYSCONF_HOST_COMPILER}" STREQUAL "GCC" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -Wuninitialized -Winit-self -ansi -pedantic" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-declarations" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch" )
endif()

if( "${CM_EBS_SYSCONF_HOST_COMPILER}" STREQUAL "MSVC" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
endif()
	
setInternal( TS3_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
setInternal( TS3_COMPONENTS_BUILD_MODE STATIC )

setInternal( TS3_BASE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}" )

setInternal( TS3_INCLUDE_DIR
        "${TS3_BASE_DIR}/external/include/"
        "${TS3_BASE_DIR}/source/dependencies/"
        "${TS3_BASE_DIR}/source/engine/components/"
        )
    
setInternal( TS3_LINK_DIR
        "${TS3_BASE_DIR}/external/lib/common"
        "${TS3_BASE_DIR}/external/lib/common/${CM_EBS_SYSCONF_TARGET_ARCH}"
        "${TS3_BASE_DIR}/external/lib/platform/${CM_EBS_SYSCONF_TARGET_LIBRARY_SUBDIR}"
        "${TS3_BASE_DIR}/external/lib/platform/${CM_EBS_SYSCONF_TARGET_LIBRARY_SUBDIR}/${CM_EBS_SYSCONF_TARGET_ARCH}"
        )

add_subdirectory( "source/dependencies" )

add_subdirectory( "source/engine/components" )
add_subdirectory( "source/engine/drivers" )
add_subdirectory( "source/engine/extensions" )
add_subdirectory( "source/samples" )
