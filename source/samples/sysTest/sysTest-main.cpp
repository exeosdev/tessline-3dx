
#include <ts3/system/display.h>
#include <ts3/system/sysContextNative.h>
#include <ts3/system/windowNative.h>
#include <ts3/system/eventSystemNative.h>
#include <ts3/system/openGL.h>

using namespace ts3::system;

int main( int pArgc, const char ** pArgv )
{
    SysContextCreateInfo sysCtxCreateInfo;
#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_SYSAPI_WIN32 )
    sysCtxCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
#elif( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_SYSAPI_X11 )
#endif
    sysCtxCreateInfo.flags = 0;

    auto sysCtx = creCreateSystemContext( sysCtxCreateInfo );
    auto dsmManager = createSysObject<DisplayManager>( sysCtx );
    auto openglDiver = createSysObject<GLSystemDriver>( dsmManager );

    // auto dsmDriver = dsmManager->createDisplayDriver( EDisplayDriverType::Generic );
    // dsmDriver->syncDisplayConfiguration();
    // auto dsmConfigDump = dsmDriver->generateConfigurationDump();
    // printf( "%s\n", dsmConfigDump.c_str() );

    openglDiver->initializePlatform();

    GLDisplaySurfaceCreateInfo surfaceCreateInfo;
    surfaceCreateInfo.windowGeometry.size = {1600,900};
    surfaceCreateInfo.windowGeometry.frameStyle = WindowFrameStyle::Caption;
    surfaceCreateInfo.visualConfig = vsxGetDefaultVisualConfigForSysWindow();

    auto displaySurface = openglDiver->createDisplaySurface( surfaceCreateInfo );

    GLRenderContextCreateInfo renderContextCreateInfo;
    renderContextCreateInfo.requiredAPIVersion = { 4, 2 };
    renderContextCreateInfo.targetAPIProfile = EGLAPIProfile::Core;
    renderContextCreateInfo.shareContext = nullptr;
    renderContextCreateInfo.flags = E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT;

    auto renderContext = openglDiver->createRenderContext( *displaySurface, renderContextCreateInfo );

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    auto evtController = createSysObject<EventController>( sysCtx );
    evtController->setActiveEventDispatcherDefault();
    auto * evtDispatcher = evtController->getEventDispatcher( CX_EVENT_DISPATCHER_ID_DEFAULT );

    bool runApp = true;

    evtDispatcher->bindEventHandler( EEventCodeIndex::AppActivityQuit, [&runApp,&displaySurface](const EventObject & pEvt) -> bool {
        // if( displaySurface )
        // {
        //     displaySurface->destroy();
        //     displaySurface = nullptr;
        // }
        runApp = false;
        return true;
    });
    evtDispatcher->bindEventHandler( EEventCodeIndex::WindowUpdateClose, [displaySurface,evtDispatcher](const EventObject & pEvt) -> bool {
        if( pEvt.eWindowUpdateClose.checkEventSource( displaySurface.get() ) )
        {
            evtDispatcher->postEventAppQuit();
        }
        return true;
    });
    evtDispatcher->bindEventHandler( EEventCodeIndex::InputKeyboardKey, [displaySurface,evtDispatcher](const EventObject & pEvt) -> bool {
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

    evtController->registerEventSource( *displaySurface );
    openglDiver->releaseInitState( *renderContext );
    renderContext->bindForCurrentThread( *displaySurface );

    auto glSysInfo = renderContext->querySystemVersionInfo();
    auto glSysInfoStr = glSysInfo.toString();
    printf("%s\n", glSysInfoStr.c_str() );

    while( runApp )
    {
        evtController->updateSysQueueAuto();
        displaySurface->clearColorBuffer();
        displaySurface->swapBuffers();
    }

    //wmgr->reset();

    return 0;
}
