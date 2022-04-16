
#include "androidSysContext.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

	namespace platform
	{

		ASessionData & androidGetASessionData( SysContext & pSysContext )
		{
			auto * androidSysContext = pSysContext.queryInterface<AndroidSysContext>();
			return androidSysContext->mNativeData.aSessionData;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
