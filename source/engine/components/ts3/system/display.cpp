
#include "displayDriver.h"
#include "displayManager.h"
#include "windowDefs.h"

namespace ts3
{

	DisplayManager::DisplayManager( ContextHandle pContext ) noexcept
	: BaseObject( pContext )
	{}

	DisplayManager::~DisplayManager() noexcept
	{
		_sysRelease();
	}

	DisplayManagerHandle DisplayManager::create( ContextHandle pContext )
	{
		auto displayManager = sysCreateObject<DisplayManager>( pContext );
		displayManager->_sysInitialize();
		return displayManager;
	}

#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
	DisplayDriverHandle DisplayManager::createDisplayDriverDXGI( const DisplayDriverCreateInfoDXGI & pCreateInfo )
	{
		return DisplayDriverDXGI::create( getHandle<DisplayManager>(), pCreateInfo );
	}
#endif

	DisplayDriverHandle DisplayManager::createDisplayDriverGeneric()
	{
		return DisplayDriverGeneric::create( getHandle<DisplayManager>() );
	}

	bool DisplayManager::validateWindowGeometry( WindowGeometry & pWindowGeometry ) const
	{
		auto originalPos = pWindowGeometry.position;
		auto originalSize = pWindowGeometry.size;
		auto & framePos = pWindowGeometry.position;
		auto & frameSize = pWindowGeometry.size;

		auto screenSize = queryDisplaySize();
		auto minWindowSize = queryMinWindowSize();

		if ( frameSize == cvWindowSizeMax )
		{
			// Window size exceeds the screen size - clamp the size.
			frameSize.x = screenSize.x;
			frameSize.y = screenSize.y;
		}
		else if ( ( frameSize.x == 0 ) || ( frameSize.y == 0 ) )
		{
			// Any dimension set to 0 means "use default size". By default,
			// we just use the ratio of the screen and 70% of its dimensions.
			frameSize.x = static_cast<uint32>( screenSize.x * 0.7f );
			frameSize.y = static_cast<uint32>( screenSize.y * 0.7f );
		}
		else
		{
			// Size of the window must be less than the size of the screen...
			frameSize.x = getMinOf( frameSize.x, screenSize.x );
			frameSize.y = getMinOf( frameSize.y, screenSize.y );

			// ... but at the same time bigger than the minimum allowed size (Win32-specific, really).
			frameSize.x = getMaxOf( frameSize.x, minWindowSize.x );
			frameSize.y = getMaxOf( frameSize.y, minWindowSize.y );
		}

		if ( ( framePos.x < 0 ) || ( framePos.y < 0 ) )
		{
			framePos.x = ( screenSize.x - frameSize.x ) / 2;
			framePos.y = ( screenSize.y - frameSize.y ) / 2;
		}
		else
		{
			int32 maxPosX = screenSize.x - frameSize.x;
			int32 maxPosY = screenSize.y - frameSize.y;
			framePos.x = getMinOf( framePos.x, maxPosX );
			framePos.y = getMinOf( framePos.y, maxPosY );
		}

		return ( framePos != originalPos ) || ( frameSize != originalSize );
	}

	DisplaySize DisplayManager::queryDisplaySize() const
	{
		DisplaySize displaySize;
		_sysQueryDisplaySize( displaySize );
		return displaySize;
	}

	DisplaySize DisplayManager::queryMinWindowSize() const
	{
		DisplaySize minWindowSize;
		_sysQueryMinWindowSize( minWindowSize );
		return minWindowSize;
	}

	bool DisplayManager::checkDisplayDriverSupport( EDsmDisplayDriverType pDriverID )
	{
		switch( pDriverID )
		{
			case EDsmDisplayDriverType::Generic:
				return true;

			case EDsmDisplayDriverType::DXGI:
				return TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI != 0;

			default:
				break;
		}

		return false;
	}

	EDsmDisplayDriverType DisplayManager::resolveDisplayDriverID( EDsmDisplayDriverType pDriverID )
	{
		EDsmDisplayDriverType resolvedDriverID = EDsmDisplayDriverType::Unknown;

		if( pDriverID == EDsmDisplayDriverType::Generic )
		{
			resolvedDriverID = EDsmDisplayDriverType::Generic;
		}
		else if( pDriverID == EDsmDisplayDriverType::Default )
		{
		#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
			resolvedDriverID = EDsmDisplayDriverType::DXGI;
		#else
			resolvedDriverID = EDsmDisplayDriverType::Generic;
		#endif
		}
	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		else if( pDriverID == EDsmDisplayDriverType::DXGI )
		{
			resolvedDriverID = EDsmDisplayDriverType::DXGI;
		}
	#endif

		return resolvedDriverID;
	}

}
