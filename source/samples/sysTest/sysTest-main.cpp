
#include <ts3/system/displayConfiguration.h>
#include <ts3/system/displaySystem.h>
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
    pGfxState.glSystemDriver = pGfxState.displayManager->mSysContext->createOpenGLSystemDriver( pGfxState.displayManager );
    pGfxState.glSystemDriver->initializePlatform();
}

void initializeGraphicsCreateSurface( GfxState & pGfxState )
{
    GLDisplaySurfaceCreateInfo surfaceCreateInfo;
    surfaceCreateInfo.frameGeometry.size = {800, 600 };
    surfaceCreateInfo.frameGeometry.style = EFrameStyle::Caption;
    surfaceCreateInfo.visualConfig = vsxGetDefaultVisualConfigForSysWindow();
    surfaceCreateInfo.runtimeVersionDesc.apiProfile = EGLAPIProfile::OpenGL;
    surfaceCreateInfo.runtimeVersionDesc.apiVersion = CX_GL_VERSION_BEST_SUPPORTED;
    surfaceCreateInfo.flags.set( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );

    pGfxState.glSurface = pGfxState.glSystemDriver->createDisplaySurface( surfaceCreateInfo );
}

void initializeGraphicsCreateContext( GfxState & pGfxState )
{
    GLRenderContextCreateInfo renderContextCreateInfo;
    renderContextCreateInfo.runtimeVersionDesc.apiProfile = EGLAPIProfile::OpenGL;
    renderContextCreateInfo.runtimeVersionDesc.apiVersion = CX_GL_VERSION_BEST_SUPPORTED;
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
    pGfxState.displayManager = pSysContext->createDisplayManager();
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
    auto sysContext = platform::createSysContext( sysContextCreateInfo );

    platform::AssetLoaderCreateInfoNativeParams aslCreateInfoNP;
    aslCreateInfoNP.relativeAssetRootDir = "../../../../tessline-3dx/assets";
    AssetLoaderCreateInfo aslCreateInfo;
    aslCreateInfo.nativeParams = &aslCreateInfoNP;
    auto assetLoader = sysContext->createAssetLoader( aslCreateInfo );

#endif

//    auto psAsset = assetLoader->openSubAsset( "shaders/GL4/fx_passthrough_ps", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );
//    auto vsAsset = assetLoader->openSubAsset( "shaders/GL4/fx_passthrough_vs", E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT );
//
//    std::string shaderCodePS;
//    psAsset->readAll( shaderCodePS );
//
//    std::string shaderCodeVS;
//    vsAsset->readAll( shaderCodeVS );

    GfxState gfxState;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    auto evtController = sysContext->createEventController();
    auto evtDispatcher = evtController->createEventDispatcher();
    evtController->setActiveEventDispatcher( *evtDispatcher );

#if( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_ANDROID )
    bool waitForDisplay = true;

    evtDispatcher->setEventHandler(
        EEventCodeIndex::AppActivityDisplayInit,
        [&waitForDisplay,&sysContext](const EventObject & pEvt) -> bool {
            waitForDisplay = false;
            return true;
        });

    while( waitForDisplay )
    {
        evtController->processEventsAuto();
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
            EEventCodeIndex::WindowUpdateDestroy,
            [evtDispatcher,&gfxState](const EventObject & pEvt) -> bool {
                if( pEvt.eWindowUpdateDestroy.checkEventSource( gfxState.glSurface.get() ) )
                {
                    // evtDispatcher->postEventAppQuit();
                }
                return true;
            });
    evtDispatcher->setEventHandler(
            EEventCodeIndex::InputKeyboardKey,
            [evtDispatcher,&gfxState](const EventObject & pEvt) -> bool {
                auto & keyMap = pEvt.eInputKeyboardKey.inputKeyboardState->keyStateMap;
                if( keyMap[EKeyCode::AltLeft] && keyMap[EKeyCode::Enter] )
                {
                	ts3DebugInterrupt();
                }
                return true;
            });

    initializeGraphics( sysContext, gfxState );

    evtController->registerPrimaryEventSource( *(gfxState.glSurface) );
    evtController->setEventSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_QUIT_ON_PRIMARY_SOURCE_DESTROY_BIT );

    gfxState.glSystemDriver->releaseInitState( *(gfxState.glContext) );
    gfxState.glContext->bindForCurrentThread( *(gfxState.glSurface) );

    auto glSysInfo = gfxState.glContext->querySystemVersionInfo();
    auto glSysInfoStr = glSysInfo.toString();
    printf("%s\n", glSysInfoStr.c_str() );

    while( runApp )
    {
        evtController->processEventsAuto();
        if( gfxState.pauseAnimation )
        {
            continue;
        }
        gfxState.glSurface->clearColorBuffer();
        gfxState.glSurface->swapBuffers();
    }

    return 0;
}
