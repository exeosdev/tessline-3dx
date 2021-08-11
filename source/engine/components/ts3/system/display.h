
#ifndef __TS3_SYSTEM_DISPLAY_H__
#define __TS3_SYSTEM_DISPLAY_H__

#include "sysContext.h"
#include "displayCommon.h"

namespace ts3
{
namespace system
{

    /// @brief
    class DisplayManager
    {
    public:
        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryDisplaySize();

        TS3_PCL_ATTR_NO_DISCARD DisplaySize queryMinWindowSize();

        TS3_PCL_ATTR_NO_DISCARD bool checkDriverSupport( EDsmDisplayDriverType pDriverID );

        TS3_PCL_ATTR_NO_DISCARD EDsmDisplayDriverType resolveDisplayDriverID( EDsmDisplayDriverType pDriverID );

        TS3_PCL_ATTR_NO_DISCARD bool checkWindowGeometry( const math::Pos2i & pWindowPosition,
                                                          const math::Size2u & pWindowSize );

        TS3_PCL_ATTR_NO_DISCARD bool validateWindowGeometry( math::Pos2i & pWindowPosition,
                                                             math::Size2u & pWindowSize );
    };

    /// @brief
    class DisplayDriver
    {
    public:
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_H__
