
#include "x11SysContext.h"
#include "x11AssetSystem.h"
#include "x11DisplaySystem.h"
#include "x11EventCore.h"
#include "x11FileManager.h"
#include "x11OpenGLDriver.h"
#include "x11WindowSystem.h"
#include <ts3/system/sysContextNative.h>
#include <ts3/system/assetSystemNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

    SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo )
    {
        return createDynamicInterfaceObject<X11SysContext>();
    }


    X11SysContext::X11SysContext()
    {
        _initializeX11ContextState();
    }

    X11SysContext::~X11SysContext() noexcept
    {
        _releaseX11ContextState();
    }

    AssetLoaderHandle X11SysContext::createAssetLoader( const AssetLoaderCreateInfo & pCreateInfo )
    {
        return platform::createFileAssetLoader( getHandle<X11SysContext>(),
                                                pCreateInfo.nativeParams.fileManager,
                                                pCreateInfo.nativeParams.relativeAssetRootDir );
    }

    DisplayManagerHandle X11SysContext::createDisplayManager()
    {
        return createSysObject<X11DisplayManager>( getHandle<X11SysContext>() );
    }

    EventControllerHandle X11SysContext::createEventController()
    {
        return createSysObject<X11EventController>( getHandle<X11SysContext>() );
    }

    FileManagerHandle X11SysContext::createFileManager()
    {
        return createSysObject<PosixFileManager>( getHandle<X11SysContext>() );
    }

    OpenGLSystemDriverHandle X11SysContext::createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager )
    {
        if( !pDisplayManager )
        {
            pDisplayManager = createDisplayManager();
        }

        return createSysObject<X11OpenGLSystemDriver>( pDisplayManager->getHandle<X11DisplayManager>() );
    }

    WindowManagerHandle X11SysContext::createWindowManager( DisplayManagerHandle pDisplayManager )
    {
        if( !pDisplayManager )
        {
            pDisplayManager = createDisplayManager();
        }

        return createSysObject<X11WindowManager>( pDisplayManager->getHandle<X11DisplayManager>() );
    }

    void X11SysContext::_initializeX11ContextState()
    {
        int thrInitStatus = ::XInitThreads();
        if( thrInitStatus == False )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        auto * xDisplay = ::XOpenDisplay( nullptr );
        if( xDisplay == nullptr )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        mNativeData.xSessionData.display = xDisplay;
        mNativeData.xSessionData.screenIndex = XDefaultScreen( mNativeData.xSessionData.display );
        mNativeData.xSessionData.rootWindowXID = XRootWindow( mNativeData.xSessionData.display, mNativeData.xSessionData.screenIndex );
        mNativeData.xSessionData.wmpDeleteWindow = XInternAtom( mNativeData.xSessionData.display, "WM_DELETE_WINDOW", False );
        mNativeData.xSessionData.sessionInfo.connectionNumber = XConnectionNumber( xDisplay );
        mNativeData.xSessionData.sessionInfo.vendorName = XServerVendor( xDisplay );
        mNativeData.xSessionData.sessionInfo.displayString = XDisplayString( xDisplay );
    }

    void X11SysContext::_releaseX11ContextState()
    {
        mNativeData.xSessionData.display = nullptr;
        mNativeData.xSessionData.screenIndex = -1;
        mNativeData.xSessionData.rootWindowXID = platform::E_X11_XID_NONE;
        mNativeData.xSessionData.wmpDeleteWindow = 0;
        mNativeData.xSessionData.sessionInfo.connectionNumber = -1;
        mNativeData.xSessionData.sessionInfo.vendorName.clear();
        mNativeData.xSessionData.sessionInfo.displayString.clear();
    }

    std::string X11SysContext::queryCurrentProcessExecutableFilePath() const
    {
        pid_t currentProcessID = getpid();

        std::string executableFilePath;
        std::string exeLink = std::string( "/proc/" ) + std::to_string( currentProcessID ) + std::string( "/exe" );

        if( char * bufferPtr = realpath( exeLink.c_str(), nullptr ) )
        {
            executableFilePath = bufferPtr;
            free( bufferPtr );
        }

        return executableFilePath;
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
