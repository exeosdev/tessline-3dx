
#include <ts3/system/displayManager.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3
{

	void SysDisplayManager::_sysInitialize()
	{}

	void SysDisplayManager::_sysRelease() noexcept
	{}

	void SysDisplayManager::_sysQueryDisplaySize( SysDisplaySize & pDisplaySize ) const
	{
		auto screenWidth = ::GetSystemMetrics( SM_CXSCREEN );
		auto screenHeight = ::GetSystemMetrics( SM_CYSCREEN );
		pDisplaySize.x = static_cast<uint32>( screenWidth );
		pDisplaySize.y = static_cast<uint32>( screenHeight );
	}

	void SysDisplayManager::_sysQueryMinWindowSize( SysDisplaySize & pMinWindowSize ) const
	{
		auto minWindowWidth = ::GetSystemMetrics( SM_CXMIN );
		auto minWindowHeight = ::GetSystemMetrics( SM_CYMIN );
		pMinWindowSize.x = static_cast<uint32>( minWindowWidth );
		pMinWindowSize.y = static_cast<uint32>( minWindowHeight );
	}

}
#endif
