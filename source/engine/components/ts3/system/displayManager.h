
#ifndef __TS3_SYSTEM_DISPLAY_MANAGER_H__
#define __TS3_SYSTEM_DISPLAY_MANAGER_H__

#include "sysContext.h"
#include "sysObject.h"
#include "displayCommon.h"

namespace ts3::system
{

    struct DisplayManagerNativeData;

    ts3SysDeclareHandle( DisplayManager );
    ts3SysDeclareHandle( DisplayManager );

    /// @brief
    class DisplayManager : public SysObject
    {
    public:
        struct ObjectPrivateData;
        std::unique_ptr<ObjectPrivateData> const mPrivate;
        const DisplayManagerNativeData * const mNativeData = nullptr;

    public:
        explicit DisplayManager( SysContextHandle pSysContext );
        virtual ~DisplayManager();

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryDefaultDisplaySize() const;

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryMinWindowSize() const;

        TS3_PCL_ATTR_NO_DISCARD bool checkDriverSupport( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD EDisplayDriverType resolveDisplayDriverID( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD bool checkWindowGeometry( const math::Pos2i & pWindowPosition,
                                                          const math::Size2u & pWindowSize ) const;

        bool validateWindowGeometry( math::Pos2i & pWindowPosition,
                                     math::Size2u & pWindowSize ) const;

    private:
        void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const;
        void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_MANAGER_H__
