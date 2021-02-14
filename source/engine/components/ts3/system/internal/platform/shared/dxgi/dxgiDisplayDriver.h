
#ifndef __TS3_SYSTEM_DXGI_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_DXGI_DISPLAY_DRIVER_H__

#include "dxgiCommon.h"

namespace ts3
{

	struct SysDisplayDriverNativeDataDXGI
	{
		ComPtr<IDXGIFactory1> dxgiFactory;
	};

	struct SysDsmAdapterNativeDataDXGI
	{
		ComPtr<IDXGIAdapter1> dxgiAdapter;
		DXGI_ADAPTER_DESC1  dxgiAdapterDesc;
	};

	struct SysDsmOutputNativeDataDXGI
	{
		ComPtr<IDXGIOutput1> dxgiOutput;
		DXGI_OUTPUT_DESC  dxgiOutputDesc;
	};

	struct SysDsmVideoModeNativeDataDXGI
	{
		DXGI_MODE_DESC dxgiModeDesc;
	};

	struct SysDisplayDriverCreateInfoDXGI
	{
		IDXGIFactory * dxgiFactory = nullptr;
	};

}

#endif // __TS3_SYSTEM_DXGI_DISPLAY_DRIVER_H__
