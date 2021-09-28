
set( APP_SAMPLE_MAIN_ENTRY_POINT_LIB "ts3sample_test1" )

link_directories( ${EXF_LINK_DIR} )

add_executable( ts3dx_boostrap_win32
        "ts3-win32-launcher.cpp" )

set_target_properties( ts3dx_boostrap_win32 PROPERTIES
        OUTPUT_NAME "ts3dx-boostrap-win32" )

target_link_libraries( ts3dx_boostrap_win32 PUBLIC
        exf_system
		ts3_eccore
		ts3_ecgraphics
        "${APP_SAMPLE_MAIN_ENTRY_POINT_LIB}" )

if( ${CM_EBS_SYSCONF_TARGET_SYSTEM} STREQUAL WinDesktop )
    target_link_libraries( ts3dx_boostrap_win32 PUBLIC
            glew32s.lib
            opengl32.lib )
endif()
