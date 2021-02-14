
#pragma once

#include <ts3/core/debug.h>
#include <ts3/core/exception.h>
#include <ts3/core/result.h>

#include <ebs/stdx/array_view.h>
#include <ebs/stdx/bitmask.h>
#include <ebs/stdx/static_algo.h>
#include <ebs/stdx/variant.h>

#if( TS3_BUILD_STATIC )
#  define TS3_SCRIPT_API
#  define TS3_SCRIPT_CLASS
#  define TS3_SCRIPT_OBJ    extern
#else
#  if( TS3_SCRIPT_BUILD )
#    define TS3_SCRIPT_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_SCRIPT_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_SCRIPT_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3_SCRIPT_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_SCRIPT_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_SCRIPT_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

namespace ts3::script
{

	TS3_SCRIPT_API void init();

}
