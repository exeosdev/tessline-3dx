
set( TS3_CDEP_png_SCR_FILES
        png.h
        png.c
        pngconf.h
        pngdebug.h
        pngerror.c
        pngget.c
        pnginfo.h
        pnglibconf.h
        pngmem.c
        pngpread.c
        pngpriv.h
        pngread.c
        pngrio.c
        pngrtran.c
        pngrutil.c
        pngset.c
        pngstruct.h
        pngtest.c
        pngtrans.c
        pngwio.c
        pngwrite.c
        pngwtran.c
        pngwutil.c
        )

add_library( src-dep-png STATIC
        ${TS3_CDEP_png_SCR_FILES} )

set_target_properties( src-dep-png PROPERTIES
        OUTPUT_NAME "png" )

target_include_directories( src-dep-png PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR} )

target_compile_definitions( src-dep-png PRIVATE
        _CRT_SECURE_NO_WARNINGS=1
        _CRT_SECURE_NO_DEPRECATED=1
)

if( "${CM_EBS_SYSCONF_HOST_COMPILER}" MATCHES "Clang|GCC" )
    target_compile_options( src-dep-png PRIVATE
            -Wno-implicit-function-declaration
    )
endif()
