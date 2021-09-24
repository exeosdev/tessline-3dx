
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_DEFS_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_DEFS_H__

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

namespace ts3::system
{

    class SysContext;

    using XDisplay = Display *;
    using XWindow = XID;
    using XColormap = XID;

    // Having these pretty common words replaced with a numbers tends
    // to be an issue, so we use some explicit enum constants instead
    // and remove them from the global scope.
    enum : XID
    {
        E_X11_XID_ALWAYS = Always,
        E_X11_XID_NONE = None,
        E_X11_XID_SUCCESS = Success,
    };

    #undef Always
    #undef None
    #undef Success

    inline constexpr auto CX_X11_DISPLAY_NULL = static_cast<XDisplay>( nullptr );

    struct X11SessionInfo
    {
        int connectionNumber;
        std::string vendorName;
        std::string displayString;
    };

    struct X11SessionData
    {
        XDisplay display = CX_X11_DISPLAY_NULL;
        XWindow rootWindowXID = E_X11_XID_NONE;
        int screenIndex = -1;
        Atom wmpDeleteWindow = 0;
        X11SessionInfo sessionInfo;

        explicit operator bool() const
        {
            return display != CX_X11_DISPLAY_NULL;
        }

        void reset()
        {
            display = CX_X11_DISPLAY_NULL;
            rootWindowXID = E_X11_XID_NONE;
            screenIndex = -1;
        }
    };

    struct X11NativeDataCommon
    {
        X11SessionData * xSessionDataPtr = nullptr;

        void setSessionData( X11SessionData & pSessionData )
        {
            ts3DebugAssert( xSessionDataPtr == nullptr );
            xSessionDataPtr = &pSessionData;
        }

        void resetSessionData()
        {
            ts3DebugAssert( xSessionDataPtr != nullptr );
            xSessionDataPtr = nullptr;
        }

        X11SessionData & getSessionData() const
        {
            ts3DebugAssert( xSessionDataPtr != nullptr );
            return *xSessionDataPtr;
        }
    };

    TS3_SYSTEM_API_NODISCARD X11SessionData & nativeX11GetXSessionData( SysContext & pSysContext );

    TS3_PCL_ATTR_NO_DISCARD inline X11SessionData & nativeX11GetXSessionData( const X11NativeDataCommon & pNativeData )
    {
        ts3DebugAssert( pNativeData.xSessionDataPtr );
        return *( pNativeData.xSessionDataPtr );
    }

    template <typename TpSysObject>
    TS3_PCL_ATTR_NO_DISCARD inline X11SessionData & nativeX11GetXSessionDataInternal( const TpSysObject & pSysObject )
    {
        ts3DebugAssert( pSysObject.mInternal->nativeDataPriv.xSessionDataPtr );
        return *( pSysObject.mInternal->nativeDataPriv.xSessionDataPtr );
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_DEFS_H__
