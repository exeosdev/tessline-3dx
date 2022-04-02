
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__

#include <ts3/system/prerequisites.h>
#import <CoreGraphics/CGError.h>

namespace ts3::system
{

    namespace platform
    {

        const char * osxQueryCGErrorMessage( CGError pCGError );

    }

    template <typename TpBaseType, typename TpNativeData>
    class OSXNativeObject : public NativeObject<TpBaseType, TpNativeData>
    {
    public:
        template <typename... TpBaseTypeArgs>
        explicit OSXNativeObject( TpBaseTypeArgs && ...pBaseTypeArgs )
        : NativeObject<TpBaseType, TpNativeData>( std::forward<TpBaseTypeArgs>( pBaseTypeArgs )... )
        {}

        virtual ~OSXNativeObject() = default;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__
