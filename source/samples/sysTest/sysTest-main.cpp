
#include <ts3/system/displayDriver.h>
#include <ts3/system/sysContextNative.h>
#include <ts3/system/windowNative.h>
#include <ts3/system/eventCoreNative.h>
#include <ts3/system/openGLDriver.h>
#include <ts3/system/assetSystemNative.h>

using namespace ts3::system;

struct GfxState
{
    DisplayManagerHandle displayManager;
    OpenGLSystemDriverHandle glSystemDriver;
    OpenGLDisplaySurfaceHandle glSurface;
    OpenGLRenderContextHandle glContext;
    bool pauseAnimation = false;
};

void initializeGraphicsCreateDriver( GfxState & pGfxState )
{
    pGfxState.glSystemDriver = createSysObject<OpenGLSystemDriver>( pGfxState.displayManager );
    pGfxState.glSystemDriver->initializePlatform();
}

void initializeGraphicsCreateSurface( GfxState & pGfxState )
{
    GLDisplaySurfaceCreateInfo surfaceCreateInfo;
    surfaceCreateInfo.windowGeometry.size = {0, 0 };
    surfaceCreateInfo.windowGeometry.frameStyle = EFrameStyle::Caption;
    surfaceCreateInfo.visualConfig = vsxGetDefaultVisualConfigForSysWindow();
    surfaceCreateInfo.flags.set( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );

    pGfxState.glSurface = pGfxState.glSystemDriver->createDisplaySurface( surfaceCreateInfo );
}

void initializeGraphicsCreateContext( GfxState & pGfxState )
{
    GLRenderContextCreateInfo renderContextCreateInfo;
    renderContextCreateInfo.requiredAPIVersion = cvGLVersionBestSupported;
    renderContextCreateInfo.targetAPIProfile = EGLAPIProfile::Core;
    renderContextCreateInfo.shareContext = nullptr;
    renderContextCreateInfo.flags = E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT;

    pGfxState.glContext = pGfxState.glSystemDriver->createRenderContext( *(pGfxState.glSurface), renderContextCreateInfo );
}

void initializeGraphicsGL( GfxState & pGfxState )
{
    initializeGraphicsCreateDriver( pGfxState );
    initializeGraphicsCreateSurface( pGfxState );
    initializeGraphicsCreateContext( pGfxState );
}

void initializeGraphics( SysContextHandle pSysContext, GfxState & pGfxState )
{
    pGfxState.displayManager = createSysObject<DisplayManager>( pSysContext );
    initializeGraphicsGL( pGfxState );
}

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )

int ts3AndroidAppMain( int argc, char ** argv, AndroidAppState * pAppState )
{
    SysContextCreateInfo sysContextCreateInfo {};
    sysContextCreateInfo.nativeParams.aCommonAppState = pAppState;
    sysContextCreateInfo.flags = 0;
    auto sysContext = nativeSysContextCreate( sysContextCreateInfo );

    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.sysContext = sysContext;
    auto assetLoader = createAssetLoader( aslCreateInfo );

#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )

int main( int pArgc, const char ** pArgv )
{
    SysContextCreateInfo sysContextCreateInfo;
    sysContextCreateInfo.flags = 0;
    sysContextCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    auto sysContext = nativeSysContextCreate( sysContextCreateInfo );

    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.sysContext = sysContext;
    aslCreateInfo.nativeParams.relativeAssetRootDir = "../../../../../tessline-3dx/assets";
    auto assetLoader = createAssetLoader( aslCreateInfo );

#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )

int main( int pArgc, const char ** pArgv )
{
    SysContextCreateInfo sysContextCreateInfo;
    sysContextCreateInfo.flags = 0;
    auto sysContext = nativeSysContextCreate( sysContextCreateInfo );

    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.sysContext = sysContext;
    aslCreateInfo.nativeParams.relativeAssetRootDir = "../../../../../tessline-3dx/assets";
    auto assetLoader = createAssetLoader( aslCreateInfo );

#endif

    auto psAsset = assetLoader->openSubAsset( "shaders/GLES3/fx_passthrough_ps", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );
    auto vsAsset = assetLoader->openSubAsset( "shaders/GLES3/fx_passthrough_vs", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );

    std::string shaderCodePS;
    psAsset->readAll( shaderCodePS );

    std::string shaderCodeVS;
    vsAsset->readAll( shaderCodeVS );

    GfxState gfxState;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    auto evtController = createSysObject<EventController>( sysContext );
    auto evtDispatcher = evtController->createEventDispatcher();
    evtController->setActiveEventDispatcher( *evtDispatcher );

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_OS_ANDROID )
    bool waitForDisplay = true;

    evtDispatcher->setEventHandler(
        EEventCodeIndex::AppActivityDisplayInit,
        [&waitForDisplay,&sysContext](const EventObject & pEvt) -> bool {
            waitForDisplay = false;
            return true;
        });

    while( waitForDisplay )
    {
        evtController->updateSysQueueAuto();
    }

    evtDispatcher->setEventHandler(
        EEventCodeIndex::AppActivityDisplayInit,
        [&gfxState](const EventObject & pEvt) -> bool {
            initializeGraphicsGL( gfxState );
            gfxState.glContext->bindForCurrentThread( *(gfxState.glSurface) );
            gfxState.pauseAnimation = false;
            return true;
        });

    evtDispatcher->setEventHandler(
        EEventCodeIndex::AppActivityDisplayTerm,
        [&gfxState](const EventObject & pEvt) -> bool {
            //gfxState.glSystemDriver->invalidate();
            gfxState.glContext = nullptr;
            gfxState.glSurface = nullptr;
            gfxState.glSystemDriver = nullptr;
            gfxState.pauseAnimation = true;
            return true;
        });
#endif

    bool runApp = true;

    evtDispatcher->setEventHandler(
            EEventCodeIndex::AppActivityQuit,
            [&runApp,&gfxState](const EventObject & pEvt) -> bool {
                runApp = false;
                return true;
            });
    evtDispatcher->setEventHandler(
            EEventCodeIndex::WindowUpdateClose,
            [evtDispatcher,&gfxState](const EventObject & pEvt) -> bool {
                if( pEvt.eWindowUpdateClose.checkEventSource( gfxState.glSurface.get() ) )
                {
                    evtDispatcher->postEventAppQuit();
                }
                return true;
            });
    evtDispatcher->setEventHandler(
            EEventCodeIndex::InputKeyboardKey,
            [evtDispatcher,&gfxState](const EventObject & pEvt) -> bool {
                if( pEvt.eInputKeyboardKey.keyCode == EKeyCode::Escape )
                {
                    evtDispatcher->postEventAppQuit();
                }
                return true;
            });

    initializeGraphics( sysContext, gfxState );

    evtController->registerEventSource( *(gfxState.glSurface) );
    gfxState.glSystemDriver->releaseInitState( *(gfxState.glContext) );
    gfxState.glContext->bindForCurrentThread( *(gfxState.glSurface) );

    auto glSysInfo = gfxState.glContext->querySystemVersionInfo();
    auto glSysInfoStr = glSysInfo.toString();
    printf("%s\n", glSysInfoStr.c_str() );

    while( runApp )
    {
        evtController->updateSysQueueAuto();
        if( gfxState.pauseAnimation )
        {
            continue;
        }
        gfxState.glSurface->clearColorBuffer();
        gfxState.glSurface->swapBuffers();
    }

    return 0;
}
