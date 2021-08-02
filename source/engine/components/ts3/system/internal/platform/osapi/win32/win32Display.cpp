
#include <ts3/system/displayManager.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3
{

	void DisplayManager::_sysInitialize()
	{}

	void DisplayManager::_sysRelease() noexcept
	{}

	void DisplayManager::_sysQueryDisplaySize( DisplaySize & pDisplaySize ) const
	{
		auto screenWidth = ::GettemMetrics( SM_CXSCREEN );
		auto screenHeight = ::GettemMetrics( SM_CYSCREEN );
		pDisplaySize.x = static_cast<uint32>( screenWidth );
		pDisplaySize.y = static_cast<uint32>( screenHeight );
	}

	void DisplayManager::_sysQueryMinWindowSize( DisplaySize & pMinWindowSize ) const
	{
		auto minWindowWidth = ::GettemMetrics( SM_CXMIN );
		auto minWindowHeight = ::GettemMetrics( SM_CYMIN );
		pMinWindowSize.x = static_cast<uint32>( minWindowWidth );
		pMinWindowSize.y = static_cast<uint32>( minWindowHeight );
	}

}
#endif
