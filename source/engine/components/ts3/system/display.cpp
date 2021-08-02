
#include "displayDriver.h"
#include "displayManager.h"
#include "windowDefs.h"

namespace ts3
{

	SysDisplayManager::SysDisplayManager( SysContextHandle pSysContext ) noexcept
	: SysBaseObject( pSysContext )
	{}

	SysDisplayManager::~SysDisplayManager() noexcept
	{
		_sysRelease();
	}

	SysDisplayManagerHandle SysDisplayManager::create( SysContextHandle pSysContext )
	{
		auto displayManager = sysCreateObject<SysDisplayManager>( pSysContext );
		displayManager->_sysInitialize();
		return displayManager;
	}

#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
	SysDisplayDriverHandle SysDisplayManager::createDisplayDriverDXGI( const SysDisplayDriverCreateInfoDXGI & pCreateInfo )
	{
		return SysDisplayDriverDXGI::create( getHandle<SysDisplayManager>(), pCreateInfo );
	}
#endif

	SysDisplayDriverHandle SysDisplayManager::createDisplayDriverGeneric()
	{
		return SysDisplayDriverGeneric::create( getHandle<SysDisplayManager>() );
	}

	bool SysDisplayManager::validateWindowGeometry( SysWindowGeometry & pWindowGeometry ) const
	{
		auto originalPos = pWindowGeometry.position;
		auto originalSize = pWindowGeometry.size;
		auto & framePos = pWindowGeometry.position;
		auto & frameSize = pWindowGeometry.size;

		auto screenSize = queryDisplaySize();
		auto minWindowSize = queryMinWindowSize();

		if ( frameSize == cvSysWindowSizeMax )
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

	SysDisplaySize SysDisplayManager::queryDisplaySize() const
	{
		SysDisplaySize displaySize;
		_sysQueryDisplaySize( displaySize );
		return displaySize;
	}

	SysDisplaySize SysDisplayManager::queryMinWindowSize() const
	{
		SysDisplaySize minWindowSize;
		_sysQueryMinWindowSize( minWindowSize );
		return minWindowSize;
	}

	bool SysDisplayManager::checkDisplayDriverSupport( ESysDsmDisplayDriverType pDriverID )
	{
		switch( pDriverID )
		{
			case ESysDsmDisplayDriverType::Generic:
				return true;

			case ESysDsmDisplayDriverType::DXGI:
				return TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI != 0;

			default:
				break;
		}

		return false;
	}

	ESysDsmDisplayDriverType SysDisplayManager::resolveDisplayDriverID( ESysDsmDisplayDriverType pDriverID )
	{
		ESysDsmDisplayDriverType resolvedDriverID = ESysDsmDisplayDriverType::Unknown;

		if( pDriverID == ESysDsmDisplayDriverType::Generic )
		{
			resolvedDriverID = ESysDsmDisplayDriverType::Generic;
		}
		else if( pDriverID == ESysDsmDisplayDriverType::Default )
		{
		#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
			resolvedDriverID = ESysDsmDisplayDriverType::DXGI;
		#else
			resolvedDriverID = ESysDsmDisplayDriverType::Generic;
		#endif
		}
	#if( TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI )
		else if( pDriverID == ESysDsmDisplayDriverType::DXGI )
		{
			resolvedDriverID = ESysDsmDisplayDriverType::DXGI;
		}
	#endif

		return resolvedDriverID;
	}

}
