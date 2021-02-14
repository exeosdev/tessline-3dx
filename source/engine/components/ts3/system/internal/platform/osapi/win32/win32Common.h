
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__

#include <comdef.h>
#include <windows.h>

namespace ts3
{

	template <typename Tp>
	using ComPtr = Microsoft::WRL::ComPtr<Tp>;

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__
