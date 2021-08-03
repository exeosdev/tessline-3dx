
#ifndef __TS3_SYSTEM_WINDOW_H__
#define __TS3_SYSTEM_WINDOW_H__

#include "windowCommon.h"

namespace ts3
{
namespace system
{

	ts3DeclareHandle( DsmDisplayManager );
	ts3DeclareHandle( WmWindow );
	ts3DeclareHandle( WmWindowManager );

	struct WmWindowCreateInfo
	{
		WmWindowProperties properties;
	};

	TS3_SYSTEM_API_NODISCARD WmWindowManagerHandle wmCreateWindowManager( DsmDisplayManagerHandle pDisplayManager );

	TS3_SYSTEM_API void wmDestroyWindowManager( WmWindowManagerHandle pWindowManager );

	TS3_SYSTEM_API_NODISCARD WmWindowHandle wmCreateWindow( WmWindowManagerHandle pWindowManager,
                                                                          const WmWindowCreateInfo & pCreateInfo );

	TS3_SYSTEM_API_NODISCARD WmWindowSize wmWindowGetClientAreaSize( WmWindowHandle pWindow );

	TS3_SYSTEM_API_NODISCARD WmWindowSize wmWindowGetFrameSize( WmWindowHandle pWindow );

	TS3_SYSTEM_API_NODISCARD bool wmWindowManagerValidateWindowGeometry( WmWindowManagerHandle pWindowManager,
                                                                                       const WmWindowGeometry & pWindowGeometry );

	TS3_SYSTEM_API bool wmWindowManagerValidateWindowGeometry( WmWindowManagerHandle pWindowManager,
                                                               WmWindowGeometry & pWindowGeometry );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_WINDOW_H__
