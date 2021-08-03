
#ifndef __TS3_SYSTEM_DISPLAY_H__
#define __TS3_SYSTEM_DISPLAY_H__

#include "coreSessionContext.h"
#include "displayCommon.h"

namespace ts3
{
namespace system
{

    TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD DsmDisplayManagerHandle dsmCreateDisplayManager( CoreSessionContextHandle pCSContext );

    TS3_SYSTEM_API void dsmDestroyDisplayManager( DsmDisplayManagerHandle pDisplayManager );

    TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD DisplaySize dsmDisplayManagerQueryDisplaySize( DsmDisplayManagerHandle pDisplayManager );

    TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD DisplaySize dsmDisplayManagerQueryMinWindowSize( DsmDisplayManagerHandle pDisplayManager );

    TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD bool dsmDisplayManagerCheckDriverSupport( EDsmDisplayDriverType pDriverID );

    TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD EDsmDisplayDriverType dsmDisplayManagerResolveDisplayDriverID( EDsmDisplayDriverType pDriverID );

    TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD bool dsmDisplayManagerValidateWindowGeometry( DsmDisplayManagerHandle pDisplayManager,
                                                                                         const WindowGeometry & pWindowGeometry );

    TS3_SYSTEM_API bool dsmDisplayManagerValidateWindowGeometry( DsmDisplayManagerHandle pDisplayManager,
                                                                 WindowGeometry & pWindowGeometry );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_H__
