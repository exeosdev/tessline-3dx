
#pragma once

#ifndef __TS3_GPUAPI_PREREQUISITES_H__
#define __TS3_GPUAPI_PREREQUISITES_H__

#include <ts3/core/prerequisites.h>
#include <ts3/core/coreEngineState.h>
#include <ts3/core/graphicsTypes.h>
#include <ts3/core/mathImports.h>

#include <memory>

#if( TS3_BUILD_STATIC )
#  define TS3_GPUAPI_API
#  define TS3_GPUAPI_CLASS
#  define TS3_GPUAPI_OBJ    extern
#else
#  if( TS3_GRAPHICS_BUILD )
#    define TS3_GPUAPI_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_GPUAPI_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_GPUAPI_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3_GPUAPI_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_GPUAPI_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_GPUAPI_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#include "prerequisites/commonDefs.h"
#include "prerequisites/commonTypes.h"
#include "prerequisites/coreInterfaceDefs.h"
#include "prerequisites/gpuBaseObject.h"
#include "prerequisites/gpuDataFormats.h"

namespace ts3::gpuapi
{

	// Same for all drivers. A top-level interface for querying capabilities and
	// creating some system-level things like a display manager or a swap chain.
	class GPUDriver;

	// OpenGL:
	// GLES: --//--
	// DX11: IDXGIAdapter + ID3D11Device
	// DX12: IDXGIAdapter + ID3D12CommandQueue
	// Vulkan: VkInstance + VkPhysicalDevice
	class GPUDevice;

	// OpenGL: context + extensions, specific version
	// GLES: --//--
	// DX11: ID3D11DeviceContext (immediate or deferred)
	// DX12: ID3D12CommandList
	// Vulkan: VkCommandBuffer (primary or secondary)
	class CommandContext;

	// Same for all drivers in terms of functionality: enumerating adapters and outputs,
	// querying display topology and supported modes. Implemented with different APIs:
	// OpenGL: ts3::DisplayDriver (Win32 API/X11+XRandR/etc.)
	// GLES: --//--
	// DX11&12: ts3::DisplayDriverDXGI (native data access)
	// Vulkan: ts3::DisplayDriverVulkan
	class DisplayManager;

	// OpenGL: window + surface, xxxSwapBuffers()
	// GLES: EGL surface + context, eglSwapBuffers()
	// DX11&12: HWND + IDXGISwapChain, Present()
	// Vulkan: native window + KHR swap chain + KHR present
	class PresentationLayer;

	ts3DeclareClassHandle( CommandContext );
	ts3DeclareClassHandle( CommandSystem );
	ts3DeclareClassHandle( DisplayManager );
	ts3DeclareClassHandle( GPUDevice );
	ts3DeclareClassHandle( GPUDriver );
	ts3DeclareClassHandle( PresentationLayer );

	enum class EGPUDriverID : uint32
	{
		GDIDDX11 = 0xCD11,
		GDIDDX12 = 0xCD12,
		GDIDGL4 = 0xA0C4,
		GDIDGLES3 = 0xA0E3,
		GDIDVK1 = 0xFF01,
		GDID0 = 0
	};

}

#endif // __TS3_GPUAPI_PREREQUISITES_H__
