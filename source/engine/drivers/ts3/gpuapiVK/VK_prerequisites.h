
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VKCOMMON_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_VKCOMMON_PREREQUISITES_H__

#include <ts3/gpuapi/memory/commonMemoryDefs.h>
#include <ts3/system/gfxVulkan.h>

#if( TS3_BUILD_STATIC )
#  define TS3GX_VKCOMMON_API
#  define TS3GX_VKCOMMON_CLASS
#  define TS3GX_VKCOMMON_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_VKCOMMON_BUILD )
#    define TS3GX_VKCOMMON_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_VKCOMMON_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_VKCOMMON_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_VKCOMMON_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_VKCOMMON_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_VKCOMMON_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#include "prerequisites/VK_commonUtils.h"
#include "prerequisites/VK_constantsMap.h"
#include "prerequisites/VK_factoryUtils.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( VKGPUDevice );
	ts3DeclareClassHandle( VKGPUDriver );
	ts3DeclareClassHandle( VKPresentationLayer );

}

#endif // __TS3DRIVER_GPUAPI_VKCOMMON_PREREQUISITES_H__
