
set( TS3_CORE_SRC_FILES
		"prerequisites/coreDefs.h"
		"prerequisites/coreEnums.h"
		"prerequisites/dynamicInterface.h"
        "prerequisites/dynamicInterface.cpp"
		"prerequisites/enumCommon.h"
		"prerequisites/result.h"

        "prerequisites.h"
        "prerequisites.cpp"

		"coreEngineState.h"
		"coreEngineState.cpp"
		"exception.h"
		"exception.cpp"
		"exceptionCommon.h"
		"exceptionResult.h"
        "graphicsTypes.h"
        "mathImports.h"

		"graphics/rectAllocator.h"
		"graphics/rectAllocator.cpp"

        "memory/commonMemoryDefs.h"

		"sync/atomicRWLock.h"
		"sync/commonSyncDefs.h"
		"sync/lockable.h"
		"sync/mutexCommon.h"
		"sync/mutexLockTypes.h"
		"sync/spinLock.h"
		"sync/syncInterface.h"

		"utility/dataProvider.h"
		"utility/gdsTypeSupportCommon.h"
		"utility/gdsTypeSupportInternal.h"
		"utility/gdsTypeSupportStd.h"
		"utility/gdsCore.h"
		)

if( "${TS3_COMPONENTS_BUILD_MODE}" STREQUAL "STATIC" )
    add_library( ts3-core STATIC ${TS3_CORE_SRC_FILES} )
    target_compile_definitions( ts3-core PRIVATE
			"TS3_BUILD_STATIC=1" )
else()
    add_library( ts3-core SHARED ${TS3_CORE_SRC_FILES} )
endif()

set_target_properties( ts3-core PROPERTIES
		OUTPUT_NAME "ts3-core"
		)

target_compile_definitions( ts3-core PRIVATE
		"TS3_CORE_BUILD=1"
		)

target_include_directories( ts3-core PUBLIC
		"${TS3_INCLUDE_DIR}"
		)

target_link_libraries( ts3-core PUBLIC
		ts3-platform
		ts3-stdext
		)
