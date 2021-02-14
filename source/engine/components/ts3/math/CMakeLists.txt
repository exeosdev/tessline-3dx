
set( TS3_MATH_SRC_FILES
        "prerequisites.h"
        "prerequisites.cpp"
        "arrayOps.h"
        "arrayOpsSIMD.inl"
        "color.h"
        "color.cpp"
        "matrix.h"
        "matrixOps.h"
        "matrixOpsSIMD.inl"
        "matrixUtils.h"
        "matrixUtilsSIMD.inl"
        "vector.h"
        "vectorOps.h"
        )

add_library( ts3-math STATIC
        ${TS3_MATH_SRC_FILES}
        )

set_target_properties( ts3-math PROPERTIES
        OUTPUT_NAME "ts3-math"
        )

target_include_directories( ts3-math PUBLIC
        ${TS3_INCLUDE_DIR}
        )

target_link_libraries( ts3-math PUBLIC
        ${TS3_LINK_INPUT}
        ts3-stdext
        )
