
#ifndef __TS3_SYSTEM_PLATFORM_SHARED_WNF_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_SHARED_WNF_COMMON_H__

#include <ts3/system/prerequisites.h>

#include <comdef.h>
#include <wrl/client.h>

namespace ts3::system
{

	template <typename TVal>
	using ComPtr = Microsoft::WRL::ComPtr<TVal>;

	namespace platform
	{

		std::string mseQueryCOMErrorMessage( HRESULT pHResult );

		std::string mseQuerySystemErrorMessage( DWORD pErrorCode );

	}

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_SHARED_WNF_COMMON_H__
