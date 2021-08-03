
#ifndef __TS3_SYSTEM_DISPLAY_H__
#define __TS3_SYSTEM_DISPLAY_H__

#include "coreSessionContext.h"
#include "displayCommon.h"

namespace ts3
{
namespace system
{

    TS3_SYSTEM_API_NODISCARD DsmDisplayManagerHandle dsmCreateDisplayManager( CoreSessionContextHandle pCSContext );

    TS3_SYSTEM_API void dsmDestroyDisplayManager( DsmDisplayManagerHandle pDisplayManager );

    TS3_SYSTEM_API_NODISCARD DisplaySize dsmDisplayManagerQueryDisplaySize( DsmDisplayManagerHandle pDisplayManager );

    TS3_SYSTEM_API_NODISCARD DisplaySize dsmDisplayManagerQueryMinWindowSize( DsmDisplayManagerHandle pDisplayManager );

    TS3_SYSTEM_API_NODISCARD bool dsmDisplayManagerCheckDriverSupport( EDsmDisplayDriverType pDriverID );

    TS3_SYSTEM_API_NODISCARD EDsmDisplayDriverType dsmDisplayManagerResolveDisplayDriverID( EDsmDisplayDriverType pDriverID );

    TS3_SYSTEM_API_NODISCARD bool dsmDisplayManagerValidateWindowGeometry( DsmDisplayManagerHandle pDisplayManager,
                                                                           const math::Pos2i & pWindowPosition,
                                                                           const math::Size2u & pWindowSize );

    TS3_SYSTEM_API bool dsmDisplayManagerValidateWindowGeometry( DsmDisplayManagerHandle pDisplayManager,
                                                                 math::Pos2i & pWindowPosition,
                                                                 math::Size2u & pWindowSize );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_H__
