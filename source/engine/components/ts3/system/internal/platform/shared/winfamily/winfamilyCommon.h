
#ifndef __TS3_SYSTEM_PLATFORM_SHARED_WINFAMILY_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_SHARED_WINFAMILY_COMMON_H__

#include <comdef.h>
#include <wrl/client.h>

namespace ts3
{

	template <typename Tp>
	using ComPtr = Microsoft::WRL::ComPtr<Tp>;

	std::string winFamilyGetHresultErrorMessage( HRESULT pHResult );

}

#endif // __TS3_SYSTEM_PLATFORM_SHARED_WINFAMILY_COMMON_H__
