
#ifndef __TS3_SYSTEM_DXGI_DISPLAY_H__
#define __TS3_SYSTEM_DXGI_DISPLAY_H__

#include "dxgiCommon.h"

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

#endif // __TS3_SYSTEM_DXGI_DISPLAY_H__
