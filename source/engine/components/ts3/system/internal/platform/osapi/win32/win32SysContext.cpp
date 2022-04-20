
#include "win32SysContext.h"
#include "win32AssetSystem.h"
#include "win32DisplaySystem.h"
#include "win32EventCore.h"
#include "win32FileManager.h"
#include "win32OpenGLDriver.h"
#include "win32WindowSystem.h"
#include <ts3/system/sysContextNative.h>
#include <ts3/system/assetSystemNative.h>
#include <process.h>
#include <tlhelp32.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    namespace platform
    {

        SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo )
        {
            return createDynamicInterfaceObject<Win32SysContext>();
        }

    }


	Win32SysContext::Win32SysContext()
	{
		_initializeWin32ContextState();
	}

	Win32SysContext::~Win32SysContext() noexcept
	{
		_releaseWin32ContextState();
	}
	
	AssetLoaderHandle Win32SysContext::createAssetLoader( const AssetLoaderCreateInfo & pCreateInfo )
	{
		return platform::createFileAssetLoader( getHandle<Win32SysContext>(),
		                                        pCreateInfo.nativeParams->fileManager,
		                                        pCreateInfo.nativeParams->relativeAssetRootDir );
	}

	DisplayManagerHandle Win32SysContext::createDisplayManager()
	{
		return createSysObject<Win32DisplayManager>( getHandle<Win32SysContext>() );
	}

	EventControllerHandle Win32SysContext::createEventController()
	{
		return createSysObject<Win32EventController>( getHandle<Win32SysContext>() );
	}

	FileManagerHandle Win32SysContext::createFileManager()
	{
		return createSysObject<Win32FileManager>( getHandle<Win32SysContext>() );
	}

	OpenGLSystemDriverHandle Win32SysContext::createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager )
	{
		if( !pDisplayManager )
		{
			pDisplayManager = createDisplayManager();
		}

		return createSysObject<Win32OpenGLSystemDriver>( pDisplayManager->getHandle<Win32DisplayManager>() );
	}

	WindowManagerHandle Win32SysContext::createWindowManager( DisplayManagerHandle pDisplayManager )
	{
		if( !pDisplayManager )
		{
			pDisplayManager = createDisplayManager();
		}

		return createSysObject<Win32WindowManager>( pDisplayManager->getHandle<Win32DisplayManager>() );
	}

	std::string Win32SysContext::queryCurrentProcessExecutableFilePath() const
	{
		std::string executableFilePath;

		DWORD validProcessID = 0;
		DWORD currentProcessID = getpid();
		HANDLE moduleSnapshotHandle = ::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, currentProcessID );

		MODULEENTRY32 moduleEntryInfo;
		moduleEntryInfo.dwSize = sizeof( MODULEENTRY32 );

		if( ::Module32First( moduleSnapshotHandle, &moduleEntryInfo ) )
		{
			do
			{
				if( moduleEntryInfo.th32ProcessID == currentProcessID )
				{
					validProcessID = currentProcessID;
					break;
				}
			}
			while( ::Module32Next( moduleSnapshotHandle, &moduleEntryInfo ) );
		}

		::CloseHandle( moduleSnapshotHandle );

		if( validProcessID != 0 )
		{
			executableFilePath = moduleEntryInfo.szExePath;
		}

		return executableFilePath;
	}

	void Win32SysContext::_initializeWin32ContextState()
	{
		mNativeData.appExecModuleHandle = ::GetModuleHandleA( nullptr );
	}

	void Win32SysContext::_releaseWin32ContextState()
	{
		mNativeData.appExecModuleHandle = nullptr;
	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
