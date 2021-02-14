
set( TS3_ENGINE_SRC_FILES
		"prerequisites/gpuapiCommon.h"
		"prerequisites/geometryCommon.h"

        "prerequisites.h"
        "prerequisites.cpp"

        "camera/cameraCommonDefs.h"
        "camera/cameraController.h"
        "camera/cameraController.cpp"

        "gpuapi/commonGPUDefs.h"
		"gpuapi/hardwareBuffer.h"
		"gpuapi/hardwareBuffer.cpp"
		"gpuapi/hardwareGeometryBuffer.h"
		"gpuapi/hardwareGeometryBuffer.cpp"
        "gpuapi/vertexFormatDefs.h"
        "gpuapi/vertexFormatDefs.cpp"

        "rcs/commonRcsUtils.h"
        "rcs/commonRcsUtils.cpp"

        "rcs/font/fontCommon.h"
        "rcs/font/font.h"
        "rcs/font/font.cpp"
        "rcs/font/fontTexture.h"
        "rcs/font/fontTexture.cpp"
        "rcs/font/fontManager.h"
        "rcs/font/fontManager.cpp"

        "rcs/image/imageCommon.h"
        "rcs/image/imageCommon.cpp"

        "rcs/mesh/mesh.h"
        "rcs/mesh/mesh.cpp"
)

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3-engine STATIC ${TS3_ENGINE_SRC_FILES} )
    target_compile_definitions( ts3-engine PRIVATE
            "TS3_BUILD_STATIC=1")
else()
    add_library( ts3-engine SHARED ${TS3_ENGINE_SRC_FILES} )
endif()

set_target_properties( ts3-engine PROPERTIES
        OUTPUT_NAME "ts3-engine" )

target_compile_definitions( ts3-engine PRIVATE
        "TS3_ENGINE_BUILD=1"
		)

target_include_directories( ts3-engine PUBLIC
		"${TS3_INCLUDE_DIR}"
		)

target_link_libraries( ts3-engine PUBLIC
        ts3-gpuapi
		)
