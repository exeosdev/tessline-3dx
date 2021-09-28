
#ifndef __TS3_SYSTEM_DXGI_DISPLAY_H__
#define __TS3_SYSTEM_DXGI_DISPLAY_H__

#include "dxgiCommon.h"

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
namespace ts3::system
{

	struct DisplayDriverNativeDataDXGI
	{
		ComPtr<IDXGIFactory1> dxgiFactory;
	};

	struct DisplayAdapterNativeDataDXGI
	{
		ComPtr<IDXGIAdapter1> dxgiAdapter;
		DXGI_ADAPTER_DESC1  dxgiAdapterDesc;
	};

	struct DisplayOutputNativeDataDXGI
	{
		ComPtr<IDXGIOutput1> dxgiOutput;
		DXGI_OUTPUT_DESC  dxgiOutputDesc;
	};

	struct DisplayVideoModeNativeDataDXGI
	{
		DXGI_MODE_DESC dxgiModeDesc;
	};

} // namespace ts3::system
#endif // TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI

#endif // __TS3_SYSTEM_DXGI_DISPLAY_H__
