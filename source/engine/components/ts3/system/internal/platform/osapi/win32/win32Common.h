
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__

#include <ts3/system/prerequisites.h>
#include <ts3/system/internal/platform/shared/mse/mseCommon.h>
#include <utility>
#include <comdef.h>
#include <windows.h>

namespace ts3::system
{

    template <typename TpBaseType, typename TpNativeData>
    class Win32NativeObject : public TpBaseType, public NativeObject<TpNativeData>
    {
    public:
        template <typename... TpBaseTypeArgs>
        Win32NativeObject( TpBaseTypeArgs && ...pBaseTypeArgs )
        : TpBaseType( std::forward<TpBaseTypeArgs>( pBaseTypeArgs )... )
        {}

        virtual ~Win32NativeObject() = default;
    };

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_COMMON_H__
