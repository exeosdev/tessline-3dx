
#include "osxSysContext.h"
#include "osxAssetSystem.h"
#include "osxDisplaySystem.h"
#include "osxFileManager.h"
#include "osxOpenGLDriver.h"
#include "osxWindowSystem.h"
#include <ts3/system/sysContextNative.h>
#include <ts3/system/assetSystemNative.h>
#include "nsOSXApplicationProxy.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
namespace ts3::system
{

    namespace platform
    {

        SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo )
        {
            return createDynamicInterfaceObject<OSXSysContext>();
        }

    }


    OSXSysContext::OSXSysContext()
    {
        _initializeOSXContextState();
    }

    OSXSysContext::~OSXSysContext() noexcept
    {
        _releaseOSXContextState();
    }
    
    AssetLoaderHandle OSXSysContext::createAssetLoader( const AssetLoaderCreateInfo & pCreateInfo )
    {
		ts3DebugAssert( pCreateInfo.nativeParams );
        return platform::createFileAssetLoader( getHandle<SysContext>(), *( pCreateInfo.nativeParams ) );
    }

    DisplayManagerHandle OSXSysContext::createDisplayManager()
    {
        return createSysObject<OSXDisplayManager>( getHandle<OSXSysContext>() );
    }

    EventControllerHandle OSXSysContext::createEventController()
    {
        return createSysObject<OSXEventController>( getHandle<OSXSysContext>() );
    }

    FileManagerHandle OSXSysContext::createFileManager()
    {
        return createSysObject<PosixFileManager>( getHandle<OSXSysContext>() );
    }

    OpenGLSystemDriverHandle OSXSysContext::createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager )
    {
        if( !pDisplayManager )
        {
            pDisplayManager = createDisplayManager();
        }

        return createSysObject<OSXOpenGLSystemDriver>( pDisplayManager->getHandle<OSXDisplayManager>() );
    }

    WindowManagerHandle OSXSysContext::createWindowManager( DisplayManagerHandle pDisplayManager )
    {
        if( !pDisplayManager )
        {
            pDisplayManager = createDisplayManager();
        }

        return createSysObject<OSXWindowManager>( pDisplayManager->getHandle<OSXDisplayManager>() );
    }

    std::string OSXSysContext::queryCurrentProcessExecutableFilePath() const
    {
        std::string executableFilePath;

        executableFilePath = "/Users/mateusz/.buildcache/cmake/source/samples/gpuapi01";

        return executableFilePath;
    }

	bool OSXSysContext::isNSAppProxyRegistered() const
	{
		return _stateMask.isSet( E_STATE_NS_APP_PROXY_REGISTERED, std::memory_order_acquire );
	}

    void OSXSysContext::_initializeOSXContextState()
    {
		_registerNSAppProxy();
    }

    void OSXSysContext::_releaseOSXContextState()
    {
    }

	void OSXSysContext::_registerNSAppProxy()
	{
	@autoreleasepool
	{
		auto & osxSharedData = platform::osxGetOSXSharedData( *this );

		if( !NSApp )
		{
			[NSOSXApplicationProxy sharedApplication];
			ts3DebugAssert( NSApp );

			if( ![NSApp delegate] )
			{
				auto * nsAppDelegate = [[NSOSXApplicationDelegate alloc] init];
				[nsAppDelegate setOSXSysContext:this];
				[NSApp setDelegate:nsAppDelegate];

				mNativeData.nsApplicationDelegate = nsAppDelegate;
				mNativeData.osxSharedData.stateFlags.isSet( platform::E_OSX_COMMON_STATE_APP_DELEGATE_INITIALIZED_BIT );
			}

			if( !mNativeData.osxSharedData.stateFlags.isSet( platform::E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT ) )
			{
				[NSApp finishLaunching];

				// Note: E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT is usually (better: almost certainly) not set at
				// this point. The reason is that [finishLaunching] posts a message which is caught by the AppDelegate
				// and processed during event loop. That means, that applicationDidFinishLaunching() may still not be
				// called when SysContext finishes its initialization process.

				if( [NSApp delegate] )
				{
					[NSOSXApplicationProxy registerUserDefaults];
				}
			}
		}
	}
	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_OSX
