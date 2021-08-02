
#include "windowSystem.h"
#include "displayManager.h"
#include "eventCore.h"
#include <ts3/stdext/utilities.h>

namespace ts3
{

	SysWindowManager::SysWindowManager( SysDisplayManagerHandle pDisplayManager ) noexcept
	: SysBaseObject( pDisplayManager->mSysContext )
	, mDisplayManager( pDisplayManager )
	{}

	SysWindowManager::~SysWindowManager() noexcept
	{
		_sysRelease();
	}

	SysWindowManagerHandle SysWindowManager::create( SysDisplayManagerHandle pDisplayManager )
	{
		auto windowManager = sysCreateObject<SysWindowManager>( pDisplayManager );
		windowManager->_sysInitialize();
		return windowManager;
	}

	SysWindowHandle SysWindowManager::createWindow( const SysWindowCreateInfo & pCreateInfo )
	{
		SysWindowCreateInfo validatedCreateInfo;
		validatedCreateInfo.properties = pCreateInfo.properties;

		// Validate the form - position and size using requested style and display properties.
		validateWindowGeometry( validatedCreateInfo.properties.geometry );

		return SysWindow::create( getHandle<SysWindowManager>(), validatedCreateInfo );
	}

	bool SysWindowManager::validateWindowGeometry( SysWindowGeometry & pWindowGeometry ) const
	{
		return mDisplayManager->validateWindowGeometry( pWindowGeometry );
	}

	SysWindowSize SysWindowManager::queryDisplaySize() const
	{
		return mDisplayManager->queryDisplaySize();
	}

	SysWindowSize SysWindowManager::queryMinWindowSize() const
	{
		return mDisplayManager->queryDisplaySize();
	}



	SysWindow::SysWindow( SysWindowManagerHandle pWindowManager ) noexcept
	: SysBaseObject( pWindowManager->mSysContext )
	, SysEventSource( mNativeData )
	, mWindowManager( pWindowManager )
	{}

	SysWindow::~SysWindow() noexcept
	{
		_sysRelease();
	}

	SysWindowHandle SysWindow::create( SysWindowManagerHandle pWindowManager, const SysWindowCreateInfo & pCreateInfo )
	{
		auto sysWindow = sysCreateObject<SysWindow>( pWindowManager );
		sysWindow->_sysInitialize( pCreateInfo );
		return sysWindow;
	}

	SysWindowSize SysWindow::getClientAreaSize() const
	{
		SysWindowSize clientAreaSize;
		_sysGetClientAreaSize( clientAreaSize );
		return clientAreaSize;
	}

	SysWindowSize SysWindow::getFrameSize() const
	{
		SysWindowSize frameSize;
		_sysGetFrameSize( frameSize );
		return frameSize;
	}


}
