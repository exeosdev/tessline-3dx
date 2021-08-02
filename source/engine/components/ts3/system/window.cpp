
#include "windowtem.h"
#include "displayManager.h"
#include "eventCore.h"
#include <ts3/stdext/utilities.h>

namespace ts3
{

	WindowManager::WindowManager( DisplayManagerHandle pDisplayManager ) noexcept
	: BaseObject( pDisplayManager->mContext )
	, mDisplayManager( pDisplayManager )
	{}

	WindowManager::~WindowManager() noexcept
	{
		_sysRelease();
	}

	WindowManagerHandle WindowManager::create( DisplayManagerHandle pDisplayManager )
	{
		auto windowManager = sysCreateObject<WindowManager>( pDisplayManager );
		windowManager->_sysInitialize();
		return windowManager;
	}

	WindowHandle WindowManager::createWindow( const WindowCreateInfo & pCreateInfo )
	{
		WindowCreateInfo validatedCreateInfo;
		validatedCreateInfo.properties = pCreateInfo.properties;

		// Validate the form - position and size using requested style and display properties.
		validateWindowGeometry( validatedCreateInfo.properties.geometry );

		return Window::create( getHandle<WindowManager>(), validatedCreateInfo );
	}

	bool WindowManager::validateWindowGeometry( WindowGeometry & pWindowGeometry ) const
	{
		return mDisplayManager->validateWindowGeometry( pWindowGeometry );
	}

	WindowSize WindowManager::queryDisplaySize() const
	{
		return mDisplayManager->queryDisplaySize();
	}

	WindowSize WindowManager::queryMinWindowSize() const
	{
		return mDisplayManager->queryDisplaySize();
	}



	Window::Window( WindowManagerHandle pWindowManager ) noexcept
	: BaseObject( pWindowManager->mContext )
	, EventSource( mNativeData )
	, mWindowManager( pWindowManager )
	{}

	Window::~Window() noexcept
	{
		_sysRelease();
	}

	WindowHandle Window::create( WindowManagerHandle pWindowManager, const WindowCreateInfo & pCreateInfo )
	{
		auto sysWindow = sysCreateObject<Window>( pWindowManager );
		sysWindow->_sysInitialize( pCreateInfo );
		return sysWindow;
	}

	WindowSize Window::getClientAreaSize() const
	{
		WindowSize clientAreaSize;
		_sysGetClientAreaSize( clientAreaSize );
		return clientAreaSize;
	}

	WindowSize Window::getFrameSize() const
	{
		WindowSize frameSize;
		_sysGetFrameSize( frameSize );
		return frameSize;
	}


}
