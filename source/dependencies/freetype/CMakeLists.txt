
set( TS3_CDEP_FREETYPE_SCR_FILES
		"include/ft2build.h"
		"src/autofit/autofit.c"
		"src/base/ftbase.c"
		"src/base/ftbbox.c"
		"src/base/ftbdf.c"
		"src/base/ftbitmap.c"
		"src/base/ftcid.c"
		"src/base/ftdebug.c"
		"src/base/ftfstype.c"
		"src/base/ftgasp.c"
		"src/base/ftglyph.c"
		"src/base/ftgxval.c"
		"src/base/ftinit.c"
		"src/base/ftmm.c"
		"src/base/ftotval.c"
		"src/base/ftpatent.c"
		"src/base/ftpfr.c"
		"src/base/ftstroke.c"
		"src/base/ftsynth.c"
		"src/base/ftsystem.c"
		"src/base/fttype1.c"
		"src/base/ftwinfnt.c"
		"src/bdf/bdf.c"
		"src/cache/ftcache.c"
		"src/cff/cff.c"
		"src/cid/type1cid.c"
		"src/gzip/ftgzip.c"
		"src/lzw/ftlzw.c"
		"src/pcf/pcf.c"
		"src/pfr/pfr.c"
		"src/psaux/psaux.c"
		"src/pshinter/pshinter.c"
		"src/psnames/psmodule.c"
		"src/raster/raster.c"
		"src/sfnt/sfnt.c"
		"src/smooth/smooth.c"
		"src/truetype/truetype.c"
		"src/type1/type1.c"
		"src/type42/type42.c"
		"src/winfonts/winfnt.c" )

add_library( dplib-freetype STATIC
        ${TS3_CDEP_FREETYPE_SCR_FILES} )

set_target_properties( dplib-freetype PROPERTIES
        OUTPUT_NAME "freetype" )

target_include_directories( dplib-freetype PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/include )

target_compile_definitions( dplib-freetype PUBLIC
		_CRT_SECURE_NO_DEPRECATE=1
		_CRT_SECURE_NO_WARNINGS=1
		_LIB=1
		FT2_BUILD_LIBRARY=1 )
