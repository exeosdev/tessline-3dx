
#pragma once

#ifndef __TS3_GPUAPI_PREREQUISITES_H__
#define __TS3_GPUAPI_PREREQUISITES_H__

#include <ts3/core/exception.h>
#include <ts3/core/coreEngineState.h>
#include <ts3/core/graphicsTypes.h>
#include <ts3/core/mathImports.h>
#include <ts3/core/utility/hfsIdentifier.h>
#include <ts3/stdext/utilities.h>

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

#define TS3_GPUAPI_API_NO_DISCARD \
	TS3_GPUAPI_API TS3_ATTR_NO_DISCARD

#define ts3driverApi( access ) access

#include "prerequisites/commonDefs.h"
#include "prerequisites/commonTypes.h"
#include "prerequisites/coreInterfaceDefs.h"
#include "prerequisites/gpuDataFormats.h"

namespace ts3::gpuapi
{

	/// @namespace cxdefs
	/// @brief GpuAPI-level constant expressions and utility functions used by those.

	/// @namespace defaults
	/// @brief GpuAPI-level default/pre-defined values/instances that can be used whenever a common case is implemented.

	/// @namespace rcutil
	/// @brief Resource Utilities, used to interact with resources and provide additional, common functionalities.

	/// @namespace smutil
	/// @brief State Management Utilities, used to provide helper methods related to GPU state management.

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

	ts3GpaDeclareClassHandle( CommandContext );
	ts3GpaDeclareClassHandle( CommandSystem );
	ts3GpaDeclareClassHandle( DisplayManager );
	ts3GpaDeclareClassHandle( GPUDevice );
	ts3GpaDeclareClassHandle( GPUDriver );
	ts3GpaDeclareClassHandle( PresentationLayer );

	enum class EGPUDriverAPI : uint32
	{
		DirectX = 0xD1,
		Metal = 0xAA,
		OpenGL = 0x77,
		Vulkan = 0xCC,
		Unknown = 0x00
	};

	namespace cxdefs
	{

		inline constexpr uint32 makeGPUDriverID( EGPUDriverAPI pDriverAPI, uint32 pAPISubVersion )
		{
			return ( ( static_cast<uint32>( pDriverAPI ) & 0xFF ) << 8 ) | ( pAPISubVersion & 0xFF );
		}

	}

	enum class EGPUDriverID : uint32
	{
		GDIDirectX11      = cxdefs::makeGPUDriverID( EGPUDriverAPI::DirectX, 0x11 ),
		GDIDirectX12      = cxdefs::makeGPUDriverID( EGPUDriverAPI::DirectX, 0x12 ),
		GDIMetal1         = cxdefs::makeGPUDriverID( EGPUDriverAPI::Metal,   0x01 ),
		GDIOpenGLDesktop4 = cxdefs::makeGPUDriverID( EGPUDriverAPI::OpenGL,  0xD4 ),
		GDIOpenGLES3      = cxdefs::makeGPUDriverID( EGPUDriverAPI::OpenGL,  0xE3 ),
		GDIVulkan10       = cxdefs::makeGPUDriverID( EGPUDriverAPI::Vulkan,  0x10 ),
		GDINull           = cxdefs::makeGPUDriverID( EGPUDriverAPI::Unknown, 0xFF ),
		GDIUnknown        = cxdefs::makeGPUDriverID( EGPUDriverAPI::Unknown, 0x00 )
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PREREQUISITES_H__
