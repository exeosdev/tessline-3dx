
set( TS3_SYSTEM_SRC_FILES
        "assetCommon.h"
        "assetSystem.h"
        "assetSystem.cpp"
        "assetSystemNative.h"
        "displayCommon.h"
        "displayCommon.cpp"
        "display.h"
        "display.cpp"
        "displayNative.h"
        "dynamicLib.h"
        "dynamicLib.cpp"
        "eventCommon.h"
        "eventCoreTypes.h"
        "eventObject.h"
        "eventSystem.h"
        "eventSystem.cpp"
        "eventSystemNative.h"
        "fileCommon.h"
        "fileCommon.cpp"
        "fileManager.h"
        "fileManager.cpp"
        "openGL.h"
        "openGL.cpp"
        "openGLCommon.h"
        "openGLCommon.cpp"
        "openGLNative.h"
        "visual.h"
        "visual.cpp"
        "perfCounter.h"
        "perfCounter.cpp"
        "prerequisites.h"
        "sysContext.h"
        "sysContext.cpp"
        "sysContextNative.h"
        "sysObject.h"
        "sysObject.cpp"
        "window.h"
        "window.cpp"
        "windowCommon.h"
        "windowNative.h"
        )

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_MSE} )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/dxgi/dxgiDisplay.h"
            "internal/platform/shared/dxgi/dxgiDisplay.cpp"
            "internal/platform/shared/mse/mseCommon.h"
            "internal/platform/shared/mse/mseCommon.cpp"
            "internal/platform/shared/mse/msePerfCounter.h"
            "internal/platform/shared/mse/msePerfCounter.cpp"
            )
endif()

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_POSIX} )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/posix/posixPerfCounter.h"
            "internal/platform/shared/posix/posixPerfCounter.cpp"
            "internal/platform/shared/posix/posixFileManager.h"
            "internal/platform/shared/posix/posixFileManager.cpp"
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_GLAPI}" STREQUAL "EGL" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/egl/eglCoreUtils.h"
            "internal/platform/shared/egl/eglCoreUtils.cpp"
            "internal/platform/shared/egl/eglOpenGL.h"
            "internal/platform/shared/egl/eglOpenGL.cpp"
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Android" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/android/androidCommon.h"
            "internal/platform/osapi/android/androidCommon.cpp"
            "internal/platform/osapi/android/androidAssetSystem.h"
            "internal/platform/osapi/android/androidAssetSystem.cpp"
            "internal/platform/osapi/android/androidDisplay.h"
            "internal/platform/osapi/android/androidDisplay.cpp"
            "internal/platform/osapi/android/androidEventSystem.h"
            "internal/platform/osapi/android/androidEventSystem.cpp"
            "internal/platform/osapi/android/androidFileManager.h"
            "internal/platform/osapi/android/androidOpenGL.h"
            "internal/platform/osapi/android/androidOpenGL.cpp"
            "internal/platform/osapi/android/androidSysContext.h"
            "internal/platform/osapi/android/androidSysContext.cpp"
            "internal/platform/osapi/android/androidWindow.h"
            "internal/platform/osapi/android/androidWindow.cpp"
            )
    set( CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG TRUE )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "WinDesktop" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            #"internal/platform/osapi/win32/win32AssetSystem.h"
            #"internal/platform/osapi/win32/win32AssetSystem.cpp"
            "internal/platform/osapi/win32/win32Common.h"
            "internal/platform/osapi/win32/win32Common.cpp"
            "internal/platform/osapi/win32/win32Display.h"
            "internal/platform/osapi/win32/win32Display.cpp"
            "internal/platform/osapi/win32/win32EventSystem.h"
            "internal/platform/osapi/win32/win32EventSystem.cpp"
            "internal/platform/osapi/win32/win32FileAPI.h"
            "internal/platform/osapi/win32/win32FileAPI.cpp"
            "internal/platform/osapi/win32/win32FileManager.h"
            "internal/platform/osapi/win32/win32FileManager.cpp"
            "internal/platform/osapi/win32/win32OpenGL.h"
            "internal/platform/osapi/win32/win32OpenGL.cpp"
            "internal/platform/osapi/win32/win32SysContext.h"
            "internal/platform/osapi/win32/win32SysContext.cpp"
            "internal/platform/osapi/win32/win32Window.h"
            "internal/platform/osapi/win32/win32Window.cpp"
            )
    #set( CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG TRUE )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Linux" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/x11/x11Common.h"
            "internal/platform/osapi/x11/x11Common.cpp"
            "internal/platform/osapi/x11/x11Display.h"
            "internal/platform/osapi/x11/x11Display.cpp"
            "internal/platform/osapi/x11/x11EventSystem.h"
            "internal/platform/osapi/x11/x11EventSystem.cpp"
            "internal/platform/osapi/x11/x11FileManager.h"
            "internal/platform/osapi/x11/x11OpenGL.h"
            "internal/platform/osapi/x11/x11OpenGL.cpp"
            "internal/platform/osapi/x11/x11SysContext.h"
            "internal/platform/osapi/x11/x11SysContext.cpp"
            "internal/platform/osapi/x11/x11Window.h"
            "internal/platform/osapi/x11/x11Window.cpp"
            )
endif()

if( NOT  "${CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG}" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/default/defaultAssetSystem.h"
            "internal/platform/default/defaultAssetSystem.cpp"
            )
endif()

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3-system STATIC ${TS3_SYSTEM_SRC_FILES} )
    target_compile_definitions( ts3-system PRIVATE
            "TS3_BUILD_STATIC=1" )
else()
    add_library( ts3-system SHARED ${TS3_SYSTEM_SRC_FILES} )
endif()

set_target_properties( ts3-system PROPERTIES
        OUTPUT_NAME "ts3-system"
        )

target_include_directories( ts3-system PUBLIC
        ${TS3_INCLUDE_DIR}
        )

target_compile_definitions( ts3-system PUBLIC
        TS3_SYSTEM_BUILD=1
        GLEW_STATIC=1
        GLEW_NO_GLU=1
        )

target_link_libraries( ts3-system PUBLIC
        ${TS3_LINK_INPUT}
        ts3-core
        )

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "WinDesktop" )
    target_link_libraries( ts3-system PUBLIC
            dplib-glew
            opengl32.lib
            DXGI.lib
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Android" )
    target_link_libraries( ts3-system PUBLIC
            android
            log
            EGL
            GLESv2
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Linux" )
    target_link_libraries( ts3-system PUBLIC
            X11
            Xrandr
            GL
            GLEW
            )
endif()

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_VULKAN} )
    target_link_libraries( ts3-system PUBLIC
            #shaderc
            #shaderc_util
            #vulkan-1
            )
endif()
