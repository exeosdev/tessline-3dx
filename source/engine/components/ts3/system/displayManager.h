
#ifndef __TS3_SYSTEM_DISPLAY_MANAGER_H__
#define __TS3_SYSTEM_DISPLAY_MANAGER_H__

#include "sysContext.h"
#include "sysObject.h"
#include "displayCommon.h"

namespace ts3::system
{

    struct DisplayManagerNativeData;

    ts3SysDeclareHandle( DisplayDriver );
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
        virtual ~DisplayManager() noexcept;

        TS3_PCL_ATTR_NO_DISCARD DisplayDriverHandle createDisplayDriver( EDisplayDriverType pDriverID );

        TS3_PCL_ATTR_NO_DISCARD bool checkDriverSupport( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD EDisplayDriverType resolveDisplayDriverID( EDisplayDriverType pDriverID ) const;

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryDefaultDisplaySize() const;

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryMinWindowSize() const;

    private:
        void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const;
        void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_MANAGER_H__
