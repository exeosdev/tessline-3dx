
#ifndef __TS3_SYSTEM_PLATFORM_SHARED_WNF_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_SHARED_WNF_COMMON_H__

#include <ts3/system/prerequisites.h>

#include <comdef.h>
#include <wrl/client.h>

namespace ts3
{

	template <typename Tp>
	using ComPtr = Microsoft::WRL::ComPtr<Tp>;

	std::string wnfQueryHresultErrorMessage( HRESULT pHResult );

}

#endif // __TS3_SYSTEM_PLATFORM_SHARED_WNF_COMMON_H__
