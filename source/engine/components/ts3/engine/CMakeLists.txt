
set( TS3_ENGINE_SRC_FILES
		"prerequisites/gpuapiCommon.h"
		"prerequisites/geometryCommon.h"
		"prerequisites/coreEngineState.h"

        "prerequisites.h"
        "prerequisites.cpp"
		"exception.h"
		"exception.cpp"

        "camera/cameraCommonDefs.h"
        "camera/cameraController.h"
        "camera/cameraController.cpp"

		"geometry/indexedMeshDataBufferBuilder.h"
		"geometry/meshData.h"
		"geometry/meshData.cpp"

        "gpuapi/commonGpaDefs.h"
		# "gpuapi/hwBufferCommon.h"
		# "gpuapi/hwBuffer.h"
		# "gpuapi/hwBuffer.cpp"
		# "gpuapi/hwBufferManager.h"
		# "gpuapi/hwBufferManager.cpp"
		# "gpuapi/hwGeometryBuffer.h"
		# "gpuapi/hwGeometryBuffer.cpp"
		# "gpuapi/vertexFormatDefs.h"
        # "gpuapi/vertexFormatDefs.cpp"
		# "gpuapi/shaderResourceDefs.h"

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
		"res/image/pngCommon.cpp"
		"res/image/pngCommon.h"
		"res/resource.h"
		"res/resource.cpp"
		"res/resourceLoader.h"
		"res/resourceLoader.cpp"
		"res/resourceManager.h"
		"res/resourceManager.cpp"

        "typeinfo/tpiEngineEnum.cpp"

		# "utility/scfCommon.h"
        # "utility/scfEntry.h"
        # "utility/scfEntry.cpp"
		# "utility/scfIndex.h"
		# "utility/scfIndex.cpp"
		# "utility/scfIndexBuilder.h"
		# "utility/scfIndexBuilder.cpp"
		# "utility/scfIOSupport.h"
		# "utility/scfIOSupport.cpp"
		# "utility/scfXMLCommon.h"
		# "utility/scfXMLCommon.cpp"
		# "utility/scfXMLReader.h"
		# "utility/scfXMLReader.cpp"
		# "utility/gridGenerator.cpp"
		# "utility/gridGenerator.h"

		"geometry/meshGroup.h"
		#"renderer/materialCommon.h"
		#"renderer/gpaSharedStateLibrary.cpp"
		#"renderer/gpaSharedStateLibrary.h"
		#"renderer/renderingContext.cpp"
		#"renderer/renderingContext.h"
		"renderer/shaderLoader.h"
		"renderer/shaderLoader.cpp"

		#"gpuapi/constantBufferProxy.cpp"
		#"gpuapi/constantBufferProxy.h"
		"geometry/dataTypesConv.h"
        "geometry/geometryStorage.h"
		"geometry/geometryStorage.cpp"
		"geometry/geometryCommonDefs.h"
		"geometry/geometryVertexFormat.h"
		"geometry/geometryVertexFormat.cpp"

		#"fx/material.h"
		#"fx/material.cpp"
		#"fx/materialCommon.h"
		#"fx/materialCommon.cpp"
		#"fx/materialLibrary.cpp"
		#"fx/materialLibrary.h"
        geometry/geometryDataFormat.cpp geometry/geometryDataFormat.h geometry/geometryManager.cpp geometry/geometryManager.h geometry/meshLoader.cpp geometry/meshLoader.h geometry/meshImporterAssimp.cpp geometry/mesh.cpp geometry/mesh.h geometry/geometryDataTransfer.cpp geometry/geometryDataTransfer.h geometry/geometryCommonDefs.cpp geometry/meshGroup.cpp geometry/geometryContainer.cpp geometry/geometryContainer.h geometry/standardVertexFormats.h geometry/meshCommon.h geometry/geometryBuffer.h)

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
		assimp
		src-dep-freetype
		src-dep-png
		ts3-core
		ts3-gpuapi
		)
