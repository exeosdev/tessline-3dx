
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PREREQUISITES_H__

#include <ts3/gpuapi/memory/commonGPUMemoryDefs.h>
#include <ts3/system/openGL.h>

#define TS3GX_GL_PLATFORM_TYPE_CORE 0x7100
#define TS3GX_GL_PLATFORM_TYPE_ES   0x7200

#define TS3GX_GL_TARGET_GL32 0x8100
#define TS3GX_GL_TARGET_GL43 0x8200
#define TS3GX_GL_TARGET_ES31 0x8300

#if( !TS3GX_GL_CONFIG_ENABLE_DSA )
#  define TS3GX_GL_CONFIG_ENABLE_DSA 0
#endif

#if( GL_PLATFORM_TYPE_ES )
#  define TS3GX_GL_PLATFORM_TYPE TS3GX_GL_PLATFORM_TYPE_ES
#else
#  define TS3GX_GL_PLATFORM_TYPE TS3GX_GL_PLATFORM_TYPE_CORE
#endif

#if defined( TS3_DRIVER_GPUAPI_GL3_BUILD )
#  define TS3GX_GL_TARGET TS3GX_GL_TARGET_GL32
#endif
#if defined( TS3_DRIVER_GPUAPI_GL4_BUILD )
#  define TS3GX_GL_TARGET TS3GX_GL_TARGET_GL43
#endif
#if defined( TS3_DRIVER_GPUAPI_GLES3_BUILD )
#  define TS3GX_GL_TARGET TS3GX_GL_TARGET_ES31
#endif

#if( TS3_BUILD_STATIC )
#  define TS3GX_GL_API
#  define TS3GX_GL_CLASS
#  define TS3GX_GL_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_GLCOMMON_BUILD )
#    define TS3GX_GL_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GL_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GL_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_GL_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GL_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GL_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#include "prerequisites/GL_commonDefs.h"
#include "prerequisites/GL_debugOutput.h"
#include "prerequisites/GL_object.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GLCommandList );
	ts3DeclareClassHandle( GLCommandSystem );
	ts3DeclareClassHandle( GLGPUDevice );
	ts3DeclareClassHandle( GLGPUDriver );
	ts3DeclareClassHandle( GLPresentationLayer );

	struct GLRenderContextSpecification
	{
		Version apiVersion;
		ESysGLAPIProfile targetProfile;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PREREQUISITES_H__
