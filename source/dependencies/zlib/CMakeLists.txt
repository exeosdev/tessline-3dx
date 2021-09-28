
set( TS3_CDEP_ZLIB_SCR_FILES
        adler32.c
        compress.c
        crc32.h
        crc32.c
        deflate.h
        deflate.c
        gzclose.c
        gzguts.h
        gzlib.c
        gzread.c
        gzwrite.c
        infback.c
        inffast.h
        inffast.c
        inffixed.h
        inflate.c
        inftrees.h
        inftrees.c
        trees.h
        trees.c
        uncompr.c
        zconf.h
        zlib.h
        zutil.h
        zutil.c
        )

add_library( dplib-zlib STATIC
        ${TS3_CDEP_ZLIB_SCR_FILES} )

set_target_properties( dplib-zlib PROPERTIES
        OUTPUT_NAME "zlib" )

target_include_directories( dplib-zlib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR} )

target_compile_definitions( dplib-zlib PRIVATE
        _CRT_SECURE_NO_WARNINGS=1
        _CRT_SECURE_NO_DEPRECATED=1
)

if( "${CM_EBS_SYSCONF_HOST_COMPILER}" MATCHES "Clang|GCC" )
    target_compile_options( dplib-zlib PRIVATE
            -Wno-implicit-function-declaration
    )
endif()
