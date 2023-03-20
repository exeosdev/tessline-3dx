
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GL4_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_GL4_PREREQUISITES_H__

#include <ts3/gpuapiGL/GL_prerequisites.h>

#if( TS3_BUILD_STATIC )
#  define TS3GX_GL4_API
#  define TS3GX_GL4_CLASS
#  define TS3GX_GL4_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_GL4_BUILD )
#    define TS3GX_GL4_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GL4_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GL4_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_GL4_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GL4_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GL4_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#define TS3GX_GL4_USE_COMPAT_API 1

#if defined( TS3GX_GL4_USE_COMPAT_API )
#  if( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_MACOS )
#    undef TS3GX_GL4_USE_COMPAT_API
#    define TS3GX_GL4_USE_COMPAT_API 1
#    pragma message( "Core OpenGL API not available on macOS. Overwriting TS3GX_GL4_USE_COMPAT_API with 0." )
#  endif
#else
#  define TS3GX_GL4_USE_COMPAT_API
#endif

#if( !TS3GX_GL4_USE_COMPAT_API )
#  pragma message( "OpenGL 4 Driver will use <Core> API." )
#else
#  pragma message( "OpenGL 4 Driver will use <Compat> API." )
#endif

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GL4GPUDevice );
	ts3DeclareClassHandle( GL4GPUDriver );

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GL4_PREREQUISITES_H__
