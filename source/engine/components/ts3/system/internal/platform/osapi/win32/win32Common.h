
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__

#include <ts3/system/prerequisites.h>
#include <ts3/system/internal/platform/shared/mse/mseCommon.h>
#include <utility>
#include <comdef.h>
#include <windows.h>

namespace ts3::system
{

    namespace platform
    {

		enum EWin32SystemWindowFlags : uint32
		{
			E_WIN32_SYSTEM_WINDOW_FLAG_WM_STATE_FULLSCREEN = 0x8000
		};

        inline constexpr UINT CX_WIN32_MESSAGE_ID_FULLSCREEN_STATE_CHANGE = 0x0477;

    }

	template <typename TpBaseType, typename TpNativeData>
	class Win32NativeObject : public NativeObject<TpBaseType, TpNativeData>
	{
	public:
		template <typename... TpBaseTypeArgs>
		Win32NativeObject( TpBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TpBaseType, TpNativeData>( std::forward<TpBaseTypeArgs>( pBaseTypeArgs )... )
		{}

		virtual ~Win32NativeObject() = default;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__
