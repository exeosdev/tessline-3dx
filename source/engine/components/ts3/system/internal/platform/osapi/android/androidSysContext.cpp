
#include <ts3/system/sysContextNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

	SysContextHandle nativeCreateSysContext( const SysContextCreateInfo & pCreateInfo )
	{
		auto sysContext = std::make_shared<SysContext>();
		auto & nativeData = sysContext->mInternal->nativeDataPriv;

		nativeData.aSessionData.aCommonAppState = pCreateInfo.nativeParams.aCommonAppState;
		nativeData.aSessionData.aCommonAppState->ts3SetUserData( E_ANDROID_APP_STATE_USER_DATA_INDEX_SYS_CONTEXT, sysContext.get() );

		return sysContext;
	}

	void nativeDestroySysContext( SysContextHandle pContext )
	{}

} // namespace ts3::system
#endif
