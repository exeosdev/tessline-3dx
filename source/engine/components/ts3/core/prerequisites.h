
#ifndef __TS3_CORE_PREREQUISITES_H__
#define __TS3_CORE_PREREQUISITES_H__

#include <ts3/platform/debug.h>
#include <ts3/stdext/arrayView.h>
#include <ts3/stdext/bitmask.h>
#include <ts3/stdext/bitUtils.h>
#include <ts3/stdext/staticLimits.h>

#if( TS3_BUILD_STATIC )
#  define TS3_CORE_API
#  define TS3_CORE_CLASS
#  define TS3_CORE_OBJ    extern
#else
#  if( TS3_CORE_BUILD )
#    define TS3_CORE_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_CORE_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_CORE_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3_CORE_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_CORE_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_CORE_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#if( TS3_DEBUG )
#  define TS3_DEBUG_CODE( pCode ) pCode
#else
#  define TS3_DEBUG_CODE( pCode )
#endif

#if( TS3_DEBUG )
#  define TS3_DEBUG_INIT( ... ) { __VA_ARGS__ }
#else
#  define TS3_DEBUG_INIT( ... )
#endif

#define friendapi protected

#include "prerequisites/coreDefs.h"
#include "prerequisites/coreEnums.h"
#include "prerequisites/result.h"
#include "prerequisites/dynamicInterface.h"

namespace ts3
{

	/// @brief Helper struct for triggering a dedicated "init-as-empty" constructor. Used whenever default ctor is not feasible.
	struct InitEmptyTag
	{
	};

	/// @brief A dedicated instance of InitEmptyTag
	inline constexpr InitEmptyTag CX_INIT_EMPTY {};

} // namespace ts3

#endif // __TS3_CORE_PREREQUISITES_H__
