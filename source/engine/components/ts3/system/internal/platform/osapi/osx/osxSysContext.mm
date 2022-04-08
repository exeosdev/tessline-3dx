
#include "osxSysContext.h"
#include "osxAssetSystem.h"
#include "osxDisplaySystem.h"
#include "osxWindowSystem.h"
#include <ts3/system/sysContextNative.h>
#include <ts3/system/assetSystemNative.h>
#include "nsOSXApplicationProxy.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
namespace ts3::system
{

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
        return nullptr;//createSysObject<OSXFileManager>( getHandle<OSXSysContext>() );
    }

    OpenGLSystemDriverHandle OSXSysContext::createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager )
    {
        if( !pDisplayManager )
        {
            pDisplayManager = createDisplayManager();
        }

        return nullptr;//createSysObject<OSXOpenGLSystemDriver>( pDisplayManager->getHandle<OSXDisplayManager>() );
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

        //;

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

		[NSOSXApplicationProxy sharedApplication];
		ts3DebugAssert( NSApp != nil );

		auto * nsAppDelegate = [[NSOSXApplicationDelegate alloc] init];
		[nsAppDelegate setOSXSysContext:this];
		[(NSApplication *)NSApp setDelegate:nsAppDelegate];
		mNativeData.nsApplicationDelegate = nsAppDelegate;

		_stateMask.set( E_STATE_NS_APP_PROXY_REGISTERED, std::memory_order_release );

		if( !osxSharedData.stateFlags.isSet( platform::E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT ) )
		{
			[NSApp finishLaunching];
		}
	}
	}


	namespace platform
	{

		SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo )
		{
			return createDynamicInterfaceObject<OSXSysContext>();
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_OSX
