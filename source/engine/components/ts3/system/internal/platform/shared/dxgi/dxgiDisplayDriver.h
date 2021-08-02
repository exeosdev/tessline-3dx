
#ifndef __TS3_SYSTEM_DXGI_DISPLAY_DRIVER_H__
#define __TS3_SYSTEM_DXGI_DISPLAY_DRIVER_H__

#include "dxgiCommon.h"

namespace ts3
{

	struct DisplayDriverNativeDataDXGI
	{
		ComPtr<IDXGIFactory1> dxgiFactory;
	};

	struct DsmAdapterNativeDataDXGI
	{
		ComPtr<IDXGIAdapter1> dxgiAdapter;
		DXGI_ADAPTER_DESC1  dxgiAdapterDesc;
	};

	struct DsmOutputNativeDataDXGI
	{
		ComPtr<IDXGIOutput1> dxgiOutput;
		DXGI_OUTPUT_DESC  dxgiOutputDesc;
	};

	struct DsmVideoModeNativeDataDXGI
	{
		DXGI_MODE_DESC dxgiModeDesc;
	};

	struct DisplayDriverCreateInfoDXGI
	{
		IDXGIFactory * dxgiFactory = nullptr;
	};

}

#endif // __TS3_SYSTEM_DXGI_DISPLAY_DRIVER_H__
