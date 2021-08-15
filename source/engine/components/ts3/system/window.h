
#ifndef __TS3_SYSTEM_WINDOW_H__
#define __TS3_SYSTEM_WINDOW_H__

#include "windowCommon.h"

namespace ts3
{
namespace system
{

	ts3DeclareHandle( DsmDisplayManager );
	ts3DeclareHandle( Window );
	ts3DeclareHandle( WindowManager );

	struct WindowCreateInfo
	{
		WmWindowProperties properties;
	};

	TS3_SYSTEM_API_NODISCARD WindowManagerHandle wmCreateWindowManager( DsmDisplayManagerHandle pDisplayManager );

	TS3_SYSTEM_API void wmDestroyWindowManager( WindowManagerHandle pWindowManager );

	TS3_SYSTEM_API_NODISCARD WindowHandle wmCreateWindow( WindowManagerHandle pWindowManager,
                                                                          const WindowCreateInfo & pCreateInfo );

	TS3_SYSTEM_API_NODISCARD WindowSize wmWindowGetClientAreaSize( WindowHandle pWindow );

	TS3_SYSTEM_API_NODISCARD WindowSize wmWindowGetFrameSize( WindowHandle pWindow );

	TS3_SYSTEM_API_NODISCARD bool wmWindowManagerValidateWindowGeometry( WindowManagerHandle pWindowManager,
                                                                                       const WindowGeometry & pWindowGeometry );

	TS3_SYSTEM_API bool wmWindowManagerValidateWindowGeometry( WindowManagerHandle pWindowManager,
                                                               WindowGeometry & pWindowGeometry );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_WINDOW_H__
