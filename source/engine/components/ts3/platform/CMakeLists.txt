
set( TS3_PLATFORM_SRC_FILES
		"_internal/environment.h"
		"_internal/include.h"
		"_internal/types.h"
		"_internal/simdext.h"

		"_internal/environment/arch/arm.h"
		"_internal/environment/arch/arm64.h"
		"_internal/environment/arch/x86.h"
		"_internal/environment/arch/x86_64.h"
		"_internal/environment/compiler/clang.h"
		"_internal/environment/compiler/gcc.h"
		"_internal/environment/compiler/icc.h"
		"_internal/environment/compiler/msvc.h"
		"_internal/environment/os/android.h"
		"_internal/environment/os/ios.h"
		"_internal/environment/os/linux.h"
		"_internal/environment/os/osx.h"
		"_internal/environment/os/windesktop.h"
		"_internal/environment/os/winuwp.h"
		
		"debug.h"
		"debug.cpp"
		"fileLocationInfo.h"
		"fileLocationInfo.cpp"
		"platform.h"
		"platform.cpp"
		)

if( "${CM_EBS_SYSCONF_TARGET_SYSTEM}" STREQUAL "Android" )
	set( TS3_PLATFORM_SRC_FILES
			${TS3_PLATFORM_SRC_FILES}
			"android/androidCommon.h"
			"android/androidNativeAppProxy.h"
			"android/androidNativeAppProxy.cpp"
			)
endif()

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3-platform STATIC ${TS3_PLATFORM_SRC_FILES} )
    target_compile_definitions( ts3-platform PRIVATE
			"TS3_BUILD_STATIC=1" )
else()
    add_library( ts3-platform SHARED ${TS3_PLATFORM_SRC_FILES} )
endif()

set_target_properties( ts3-platform PROPERTIES
		OUTPUT_NAME "ts3-platform"
		)

target_compile_definitions( ts3-platform PRIVATE
		"TS3_PLATFORM_BUILD=1"
		)

target_include_directories( ts3-platform PUBLIC
		"${TS3_INCLUDE_DIR}"
		)

target_link_libraries( ts3-platform PUBLIC
		)
