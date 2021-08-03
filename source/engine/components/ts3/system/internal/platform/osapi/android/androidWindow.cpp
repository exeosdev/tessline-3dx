
#include <ts3/system/windowtem.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3
{

	void WmWindowManager::_sysInitialize()
	{}

	void WmWindowManager::_sysRelease() noexcept
	{}


	void Window::_sysInitialize( const WmWindowCreateInfo & pCreateInfo )
	{}

	void Window::_sysRelease() noexcept
	{}

	void Window::_sysGetClientAreaSize( WmWindowSize & pClientAreaSize ) const
	{}

	void Window::_sysGetFrameSize( WmWindowSize & pFrameSize ) const
	{}

}
#endif
