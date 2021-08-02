
#include <ts3/system/windowtem.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3
{

	void WindowManager::_sysInitialize()
	{}

	void WindowManager::_sysRelease() noexcept
	{}


	void Window::_sysInitialize( const WindowCreateInfo & pCreateInfo )
	{}

	void Window::_sysRelease() noexcept
	{}

	void Window::_sysGetClientAreaSize( WindowSize & pClientAreaSize ) const
	{}

	void Window::_sysGetFrameSize( WindowSize & pFrameSize ) const
	{}

}
#endif
