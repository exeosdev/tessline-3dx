
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLES3_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_GLES3_PREREQUISITES_H__

#include <ts3/gpuapiGL/GL_prerequisites.h>

#if( TS3_BUILD_STATIC )
#  define TS3GX_GLES3_API
#  define TS3GX_GLES3_CLASS
#  define TS3GX_GLES3_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_GLES3_BUILD )
#    define TS3GX_GLES3_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GLES3_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GLES3_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_GLES3_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GLES3_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GLES3_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GLES3CommandContext );
	ts3DeclareClassHandle( GLES3CommandSystem );
	ts3DeclareClassHandle( GLES3GPUDevice );
	ts3DeclareClassHandle( GLES3GPUDriver );
	ts3DeclareClassHandle( GLPresentationLayer );


}

#endif // __TS3DRIVER_GPUAPI_GLES3_PREREQUISITES_H__
