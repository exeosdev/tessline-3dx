
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VK10_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_VK10_PREREQUISITES_H__

#include <ts3/gpuapiVK/VK_prerequisites.h>

#if( TS3_BUILD_STATIC )
#  define TS3GX_VK10_API
#  define TS3GX_VK10_CLASS
#  define TS3GX_VK10_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_VK10_BUILD )
#    define TS3GX_VK10_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_VK10_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_VK10_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_VK10_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_VK10_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_VK10_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

namespace ts3::gpuapi
{

	// ts3DeclareClassHandle( VK10CommandContext );
	// ts3DeclareClassHandle( VK10GPUCmdManager );
	// ts3DeclareClassHandle( VK10GPUDevice );
	// ts3DeclareClassHandle( VK10GPUDriver );
	// ts3DeclareClassHandle( VK10PresentationLayer );

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_VK10_PREREQUISITES_H__
