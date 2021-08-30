
set( TS3_SYSTEM_SRC_FILES
        "sysContext.h"
        "sysContext.cpp"
        "sysContextNative.h"
        "sysContextNative.cpp"
        "displayCommon.h"
        "displayCommon.cpp"
        "displayDriver.h"
        "displayDriver.cpp"
        "displayDriverNative.h"
        "displayManager.h"
        "displayManager.cpp"
        "displayManagerNative.h"
        "dynamicLib.h"
        "dynamicLib.cpp"
        "eventCommon.h"
        "eventCore.h"
        "eventCore.cpp"
        "eventObject.h"
        "eventTypes.h"
        "openGL.h"
        "openGL.cpp"
        "openGLCommon.h"
        "openGLNative.h"
        "openGLNative.cpp"
        "visual.h"
        "visual.cpp"
        "perfCounter.h"
        "perfCounter.cpp"
        "prerequisites.h"
        "window.h"
        "window.cpp"
        "windowCommon.h"
        "windowNative.h"
        )

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSF_WINFAMILY} )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/dxgi/dxgiDisplayDriver.h"
            "internal/platform/shared/dxgi/dxgiDisplayDriver.cpp"
            "internal/platform/shared/wnf/wnfCommon.h"
            "internal/platform/shared/wnf/wnfCommon.cpp"
            "internal/platform/shared/wnf/wnfPerfCounter.h"
            "internal/platform/shared/wnf/wnfPerfCounter.cpp"
            )
endif()

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSF_POSIX} )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/posix/posixPerfCounter.h"
            "internal/platform/shared/posix/posixPerfCounter.cpp"
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_FLAG_OSF_GLAPI}" STREQUAL "EGL" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/egl/eglOpenGL.h"
            "internal/platform/shared/egl/eglOpenGL.cpp"
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Android" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/android/androidCommon.h"
            "internal/platform/osapi/android/androidEventCore.h"
            "internal/platform/osapi/android/androidEventCore.cpp"
            "internal/platform/osapi/android/androidOpenGL.h"
            "internal/platform/osapi/android/androidOpenGL.cpp"
            "internal/platform/osapi/android/androidWindow.h"
            "internal/platform/osapi/android/androidWindow.cpp"
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "WinDesktop" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/win32/win32Common.h"
            "internal/platform/osapi/win32/win32Common.cpp"
            "internal/platform/osapi/win32/win32SysContext.h"
            "internal/platform/osapi/win32/win32SysContext.cpp"
            "internal/platform/osapi/win32/win32DisplayDriver.h"
            "internal/platform/osapi/win32/win32DisplayDriver.cpp"
            "internal/platform/osapi/win32/win32EventCore.h"
            "internal/platform/osapi/win32/win32EventCore.cpp"
            "internal/platform/osapi/win32/win32OpenGL.h"
            "internal/platform/osapi/win32/win32OpenGL.cpp"
            "internal/platform/osapi/win32/win32Window.h"
            "internal/platform/osapi/win32/win32Window.cpp"
            )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Linux" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/x11/x11CommonDefs.h"
            "internal/platform/osapi/x11/x11SysContext.h"
            "internal/platform/osapi/x11/x11SysContext.cpp"
            "internal/platform/osapi/x11/x11Display.h"
            "internal/platform/osapi/x11/x11Display.cpp"
            "internal/platform/osapi/x11/x11DisplayXRR.h"
            "internal/platform/osapi/x11/x11DisplayXRR.cpp"
            "internal/platform/osapi/x11/x11EventCore.h"
            "internal/platform/osapi/x11/x11EventCore.cpp"
            "internal/platform/osapi/x11/x11OpenGL.h"
            "internal/platform/osapi/x11/x11OpenGL.cpp"
            "internal/platform/osapi/x11/x11Window.h"
            "internal/platform/osapi/x11/x11Window.cpp"
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
        OUTPUT_NAME "exf-system"
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

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSF_VULKAN} )
    target_link_libraries( ts3-system PUBLIC
            #shaderc
            #shaderc_util
            #vulkan-1
            )
endif()
