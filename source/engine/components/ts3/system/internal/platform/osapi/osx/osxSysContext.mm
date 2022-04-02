
#include "osxSysContext.h"
#include "osxAssetSystem.h"
#include "osxDisplaySystem.h"
#include "osxWindowSystem.h"
#include <ts3/system/sysContextNative.h>
#include <ts3/system/assetSystemNative.h>

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
        return nullptr;//createSysObject<OSXEventController>( getHandle<OSXSysContext>() );
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

    void OSXSysContext::_initializeOSXContextState()
    {
    }

    void OSXSysContext::_releaseOSXContextState()
    {
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
