
#include <ts3/system/display.h>
#include <ts3/system/sysContextNative.h>
#include <ts3/system/windowNative.h>
#include <ts3/system/eventSystemNative.h>
#include <ts3/system/openGL.h>

using namespace ts3::system;

struct GfxState
{
    DisplayManagerHandle displayManager;
    GLSystemDriverHandle glSystemDriver;
    GLDisplaySurfaceHandle glSurface;
    GLRenderContextHandle glContext;
};

void initializeGraphics( SysContextHandle pSysContext, GfxState & pGfxState )
{
    pGfxState.displayManager = createSysObject<DisplayManager>( pSysContext );
    pGfxState.glSystemDriver = createSysObject<GLSystemDriver>( pGfxState.displayManager );

    pGfxState.glSystemDriver->initializePlatform();

    GLDisplaySurfaceCreateInfo surfaceCreateInfo;
    surfaceCreateInfo.windowGeometry.size = {0, 0 };
    surfaceCreateInfo.windowGeometry.frameStyle = WindowFrameStyle::Caption;
    surfaceCreateInfo.visualConfig = vsxGetDefaultVisualConfigForSysWindow();
    surfaceCreateInfo.flags.set( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );

    pGfxState.glSurface = pGfxState.glSystemDriver->createDisplaySurface( surfaceCreateInfo );

    GLRenderContextCreateInfo renderContextCreateInfo;
    renderContextCreateInfo.requiredAPIVersion = cvGLVersionBestSupported;
    renderContextCreateInfo.targetAPIProfile = EGLAPIProfile::Core;
    renderContextCreateInfo.shareContext = nullptr;
    renderContextCreateInfo.flags = E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT;

    pGfxState.glContext = pGfxState.glSystemDriver->createRenderContext( *(pGfxState.glSurface), renderContextCreateInfo );
}

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_OS_ANDROID )
int ts3AndroidAppMain( int argc, char ** argv, AndroidAppState * pAppState )
{
    ts3::system::SysContextCreateInfo sysContextCreateInfo {};
    sysContextCreateInfo.nativeParams.aCommonAppState = pAppState;
    sysContextCreateInfo.flags = 0;
    auto sysContext = nativeCreateSysContext( sysContextCreateInfo );
#elif( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_SYSAPI_WIN32 )
int main( int pArgc, const char ** pArgv )
{
    SysContextCreateInfo sysContextCreateInfo;
    sysContextCreateInfo.flags = 0;
    sysContextCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    auto sysContext = nativeCreateSysContext( sysContextCreateInfo );
#elif( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_SYSAPI_X11 )
int main( int pArgc, const char ** pArgv )
{
    SysContextCreateInfo sysContextCreateInfo;
    sysContextCreateInfo.flags = 0;
    auto sysContext = nativeCreateSysContext( sysContextCreateInfo );
#endif

    GfxState gfxState;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    auto evtController = createSysObject<EventController>( sysContext );
    evtController->setActiveEventDispatcherDefault();
    auto * evtDispatcher = evtController->getEventDispatcher( CX_EVENT_DISPATCHER_ID_DEFAULT );

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_OS_ANDROID )
    bool waitForDisplay = true;

    evtDispatcher->bindEventHandler(
        EEventCodeIndex::AppActivityDisplayReady,
        [&waitForDisplay,&sysContext](const EventObject & pEvt) -> bool {
            waitForDisplay = false;
            return true;
        });

    while( waitForDisplay )
    {
        evtController->updateSysQueueAuto();
    }

    evtDispatcher->bindEventHandler( EEventCodeIndex::AppActivityDisplayReady, nullptr );
#endif

    bool runApp = true;

    evtDispatcher->bindEventHandler(
            EEventCodeIndex::AppActivityQuit,
            [&runApp,&gfxState](const EventObject & pEvt) -> bool {
                // if( displaySurface )
                // {
                //     displaySurface->destroy();
                //     displaySurface = nullptr;
                // }
                runApp = false;
                return true;
            });
    evtDispatcher->bindEventHandler(
            EEventCodeIndex::WindowUpdateClose,
            [evtDispatcher,&gfxState](const EventObject & pEvt) -> bool {
                if( pEvt.eWindowUpdateClose.checkEventSource( gfxState.glSurface.get() ) )
                {
                    evtDispatcher->postEventAppQuit();
                }
                return true;
            });
    evtDispatcher->bindEventHandler(
            EEventCodeIndex::InputKeyboardKey,
            [evtDispatcher,&gfxState](const EventObject & pEvt) -> bool {
                if( pEvt.eInputKeyboardKey.keyCode == EKeyCode::Escape )
                {
                    evtDispatcher->postEventAppQuit();
                }
                else if( pEvt.eInputKeyboardKey.keyCode == EKeyCode::CharF )
                {
                    //appWindow->setFullscreenMode( true );
                }
                else if( pEvt.eInputKeyboardKey.keyCode == EKeyCode::CharG )
                {
                    //appWindow->setFullscreenMode( false );
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
        gfxState.glSurface->clearColorBuffer();
        gfxState.glSurface->swapBuffers();
    }

    //wmgr->reset();

    return 0;
}
