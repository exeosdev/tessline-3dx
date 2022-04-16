
#include "x11SysContext.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

	namespace platform
	{

		X11SessionData & x11GetXSessionData( SysContext & pSysContext )
		{
			auto * x11SysContext = pSysContext.queryInterface<X11SysContext>();
			return x11SysContext->mNativeData.xSessionData;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
