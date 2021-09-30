
set( TS3_EXTENSION_RCSUPPORT_SRC_FILES

        "fonts/fontTypeBitmap.h"
        "fonts/fontTypeBitmap.cpp"
        "fonts/fontTypeFTF.h"
        "fonts/fontTypeFTF.cpp"
        "fonts/freeTypeCommon.h"
        "fonts/freeTypeFontFace.h"
        "fonts/freeTypeFontFace.cpp"
        "fonts/freeTypeFontObject.h"
        "fonts/freeTypeFontObject.cpp"

        "images/bitmapCommon.h"
        "images/bitmapCommon.cpp"

        )


if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3ext-rcsupport STATIC ${TS3_EXTENSION_RCSUPPORT_SRC_FILES} )
    target_compile_definitions( ts3ext-rcsupport PRIVATE
            "TS3_BUILD_STATIC=1")
else()
    add_library( ts3ext-rcsupport SHARED ${TS3_EXTENSION_RCSUPPORT_SRC_FILES} )
endif()

set_target_properties( ts3ext-rcsupport PROPERTIES
        OUTPUT_NAME "ts3ext-rcsupport" )

target_compile_definitions( ts3ext-rcsupport PRIVATE
        )

target_include_directories( ts3ext-rcsupport PUBLIC
        ${TS3_EXTENSIONS_INCLUDE_DIR_INTERNAL} )

target_link_libraries( ts3ext-rcsupport PUBLIC
        dplib-freetype
        ts3-core
        ts3-engine
        ts3-gpuapi )
