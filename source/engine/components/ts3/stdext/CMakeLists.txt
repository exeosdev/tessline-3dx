
set( TS3_STDEXT_SRC_FILES
        "prerequisites.h"
        "bitmask.h"
        "bitmaskAtomic.h"
        "byteBuffer.h"
        "chrono.h"
        "gdsCommon.h"
        "gdsStdCore.h"
        "gdsStdSTL.h"
        "gdsUtilities.h"
        "hash.h"
        "hash.cpp"
        "memory.h"
        "memory.cpp"
        "memoryBuffer.h"
        "pathNameIterator.h"
        "range.h"
        "sortedArray.h"
        "staticAlgo.h"
        "staticLimits.h"
        "stringExt.h"
        "stringUtils.h"
        "typeTraits.h"
        "utilities.h"
        )

add_library( ts3-stdext STATIC
        ${TS3_STDEXT_SRC_FILES}
        )

set_target_properties( ts3-stdext PROPERTIES
        OUTPUT_NAME "ts3-stdext"
        )

target_include_directories( ts3-stdext PUBLIC
        "${TS3_INCLUDE_DIR}"
        )

target_link_libraries( ts3-stdext PUBLIC
        dplib-zlib
        ts3-platform
        )
        