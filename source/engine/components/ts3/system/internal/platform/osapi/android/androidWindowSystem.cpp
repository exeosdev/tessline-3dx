
#include <ts3/system/windowSystem.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3
{

	void SysWindowManager::_sysInitialize()
	{}

	void SysWindowManager::_sysRelease() noexcept
	{}


	void SysWindow::_sysInitialize( const SysWindowCreateInfo & pCreateInfo )
	{}

	void SysWindow::_sysRelease() noexcept
	{}

	void SysWindow::_sysGetClientAreaSize( SysWindowSize & pClientAreaSize ) const
	{}

	void SysWindow::_sysGetFrameSize( SysWindowSize & pFrameSize ) const
	{}

}
#endif
