
set( TS3_ENGINE_SRC_FILES
		"prerequisites/gpuapiCommon.h"
		"prerequisites/geometryCommon.h"

        "prerequisites.h"
        "prerequisites.cpp"
		"exception.h"
		"exception.cpp"

        "camera/cameraCommonDefs.h"
        "camera/cameraController.h"
        "camera/cameraController.cpp"

        # "gpuapi/commonGPUDefs.h"
		# "gpuapi/hwBufferCommon.h"
		# "gpuapi/hwBuffer.h"
		# "gpuapi/hwBuffer.cpp"
		# "gpuapi/hwBufferManager.h"
		# "gpuapi/hwBufferManager.cpp"
		# "gpuapi/hwGeometryBuffer.h"
		# "gpuapi/hwGeometryBuffer.cpp"
		# "gpuapi/vertexFormatDefs.h"
        # "gpuapi/vertexFormatDefs.cpp"

		"renderer/commonRendererDefs.h"
		"renderer/renderer.h"
		"renderer/renderer.cpp"
		"renderer/shaderLibrary.h"
		"renderer/shaderLibrary.cpp"
		"renderer/effects/shadowRenderer.h"
		"renderer/effects/shadowRenderer.cpp"

		"res/font.h"
		"res/font.cpp"
		"res/fontCommon.h"
		"res/fontMetrics.h"
		"res/font/fontTypeDynamic.h"
		"res/font/fontTypeDynamic.cpp"
		"res/font/fontTypeStatic.h"
		"res/font/fontTypeStatic.cpp"
		"res/font/ftdFreeTypeCommon.h"
		"res/font/ftdFreeTypeFontFace.h"
		"res/font/ftdFreeTypeFontFace.cpp"
		"res/font/ftdFreeTypeFontObject.h"
		"res/font/ftdFreeTypeFontObject.cpp"
		"res/imageCommon.h"
		"res/imageCommon.cpp"
		"res/image/bitmapCommon.h"
		"res/image/bitmapCommon.cpp"
		"res/resource.h"
		"res/resource.cpp"
		"res/resourceLoader.h"
		"res/resourceLoader.cpp"
		"res/resourceManager.h"
		"res/resourceManager.cpp"

        "typeinfo/tpiEngineEnum.cpp"

		"utility/scfCommon.h"
        "utility/scfEntry.h"
        "utility/scfEntry.cpp"
		"utility/scfIndex.h"
		"utility/scfIndex.cpp"
		"utility/scfIndexBuilder.h"
		"utility/scfIndexBuilder.cpp"
		"utility/scfIOSupport.h"
		"utility/scfIOSupport.cpp"
		"utility/scfXMLCommon.h"
		"utility/scfXMLCommon.cpp"
		"utility/scfXMLReader.h"
		"utility/scfXMLReader.cpp"
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
		dplib-freetype
		ts3-core
		ts3-gpuapi
		)
