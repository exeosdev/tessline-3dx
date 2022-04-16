
set( TS3_SYSTEM_SRC_FILES
        "assetCommon.h"
        "assetSystem.h"
        "assetSystem.cpp"
        "assetSystemNative.h"
        "displayCommon.h"
        "displayCommon.cpp"
        "displayConfiguration.h"
        "displayConfiguration.cpp"
        "displaySystem.h"
        "displaySystem.cpp"
        "displaySystemNative.h"
        "dynamicLib.h"
        "dynamicLib.cpp"
        "eventCommon.h"
        "eventCore.h"
        "eventCore.cpp"
        "eventDefApp.h"
        "eventDefInputKeyboard.h"
        "eventDefInputMouse.h"
        "eventDefInputTouch.h"
        "eventObject.h"
        "eventSource.h"
        "eventSource.cpp"
        "eventCoreNative.h"
        "fileCommon.h"
        "fileCommon.cpp"
        "fileManager.h"
        "fileManager.cpp"
        "frame.h"
        "frame.cpp"
        "openGLCommon.h"
        "openGLCommon.cpp"
        "openGLDriver.h"
        "openGLDriver.cpp"
        "openGLNative.h"
        "visual.h"
        "visual.cpp"
        "perfCounter.h"
        "perfCounter.cpp"
        "prerequisites.h"
        "sysContext.h"
        "sysContext.cpp"
        "sysObject.h"
        "sysObject.cpp"
        "windowCommon.h"
        "windowSystem.h"
        "windowSystem.cpp"
        "windowNative.h"

        "internal/displaySystemPrivate.h"
        "internal/eventCorePrivate.h"
        "openGLDriver.h"
        )

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_MSE} )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/dxgi/dxgiDisplaySystem.h"
            "internal/platform/shared/dxgi/dxgiDisplaySystem.cpp"
            "internal/platform/shared/mse/mseCommon.h"
            "internal/platform/shared/mse/mseCommon.cpp"
            "internal/platform/shared/mse/msePerfCounter.h"
            "internal/platform/shared/mse/msePerfCounter.cpp"
            )
endif()

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_POSIX} )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/shared/posix/posixCommon.h"
            "internal/platform/shared/posix/posixCommon.cpp"
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
            "internal/platform/osapi/android/androidAssetSystem.h"
            "internal/platform/osapi/android/androidAssetSystem.cpp"
            "internal/platform/osapi/android/androidCommon.h"
            "internal/platform/osapi/android/androidCommon.cpp"
            "internal/platform/osapi/android/androidDisplaySystem.h"
            "internal/platform/osapi/android/androidDisplaySystem.cpp"
            "internal/platform/osapi/android/androidEventCore.h"
            "internal/platform/osapi/android/androidEventCore.cpp"
            "internal/platform/osapi/android/androidFileManager.h"
            "internal/platform/osapi/android/androidOpenGLDriver.h"
            "internal/platform/osapi/android/androidOpenGLDriver.cpp"
            "internal/platform/osapi/android/androidSysContext.h"
            "internal/platform/osapi/android/androidSysContext.cpp"
            "internal/platform/osapi/android/androidWindowSystem.h"
            "internal/platform/osapi/android/androidWindowSystem.cpp"
            "internal/platform/shared/java/javaCommon.h"
            "internal/platform/shared/java/javaNativeInterface.h"
            "internal/platform/shared/java/javaNativeInterface.cpp"
            "internal/platform/shared/java/javaVM.h"
            "internal/platform/shared/java/javaVM.cpp"
            )
    set( CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG TRUE )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "WinDesktop" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/win32/win32AssetSystem.h"
            "internal/platform/osapi/win32/win32Common.h"
            "internal/platform/osapi/win32/win32Common.cpp"
            "internal/platform/osapi/win32/win32DisplaySystem.h"
            "internal/platform/osapi/win32/win32DisplaySystem.cpp"
            "internal/platform/osapi/win32/win32EventCore.h"
            "internal/platform/osapi/win32/win32EventCore.cpp"
            "internal/platform/osapi/win32/win32FileAPI.h"
            "internal/platform/osapi/win32/win32FileAPI.cpp"
            "internal/platform/osapi/win32/win32FileManager.h"
            "internal/platform/osapi/win32/win32FileManager.cpp"
            "internal/platform/osapi/win32/win32OpenGLDriver.h"
            "internal/platform/osapi/win32/win32OpenGLDriver.cpp"
            "internal/platform/osapi/win32/win32SysContext.h"
            "internal/platform/osapi/win32/win32SysContext.cpp"
            "internal/platform/osapi/win32/win32WindowSystem.h"
            "internal/platform/osapi/win32/win32WindowSystem.cpp"
            )
    set( CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG FALSE )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Linux" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/x11/x11AssetSystem.h"
            "internal/platform/osapi/x11/x11Common.h"
            "internal/platform/osapi/x11/x11Common.cpp"
            "internal/platform/osapi/x11/x11DisplaySystem.h"
            "internal/platform/osapi/x11/x11DisplaySystem.cpp"
            "internal/platform/osapi/x11/x11EventCore.h"
            "internal/platform/osapi/x11/x11EventCore.cpp"
            "internal/platform/osapi/x11/x11FileManager.h"
            "internal/platform/osapi/x11/x11OpenGLDriver.h"
            "internal/platform/osapi/x11/x11OpenGLDriver.cpp"
            "internal/platform/osapi/x11/x11SysContext.h"
            "internal/platform/osapi/x11/x11SysContext.cpp"
            "internal/platform/osapi/x11/x11WindowSystem.h"
            "internal/platform/osapi/x11/x11WindowSystem.cpp"
            )
    set( CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG FALSE )
endif()

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "macOS" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/osapi/osx/nsOpenGLViewProxy.h"
            "internal/platform/osapi/osx/nsOpenGLViewProxy.mm"
            "internal/platform/osapi/osx/nsWindowProxy.h"
            "internal/platform/osapi/osx/nsWindowProxy.mm"
            "internal/platform/osapi/osx/osxAssetSystem.h"
            "internal/platform/osapi/osx/osxCommon.h"
            "internal/platform/osapi/osx/osxCommon.mm"
            "internal/platform/osapi/osx/osxDisplaySystem.h"
            "internal/platform/osapi/osx/osxDisplaySystem.mm"
            "internal/platform/osapi/osx/osxWindowSystem.h"
            "internal/platform/osapi/osx/osxWindowSystem.mm"
            #"internal/platform/osapi/osx/osxEventCore.h"
            #"internal/platform/osapi/osx/osxEventCore.cpp"
            #"internal/platform/osapi/osx/osxFileManager.h"
            #"internal/platform/osapi/osx/osxOpenGLDriver.h"
            #"internal/platform/osapi/osx/osxOpenGLDriver.cpp"
            #"internal/platform/osapi/osx/osxSysContext.h"
            #"internal/platform/osapi/osx/osxSysContext.cpp"
            )
    set( CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG FALSE )
endif()

if( NOT "${CM_EBS_SYSTEM_ASSET_SYSTEM_SRC_FLAG}" )
    set( TS3_SYSTEM_SRC_FILES
            ${TS3_SYSTEM_SRC_FILES}
            "internal/platform/common/fileBasedAssetSystem.h"
            "internal/platform/common/fileBasedAssetSystem.cpp"
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

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "macOS" )
    target_link_libraries( ts3-system PUBLIC
            -framework AppKit
            -framework CoreFoundation
            -framework Foundation
            )
endif()

if( ${CM_EBS_SYSCONF_TARGET_FLAG_OSINFO_VULKAN} )
    target_link_libraries( ts3-system PUBLIC
            #shaderc
            #shaderc_util
            #vulkan-1
            )
endif()
