
#include <ts3/system/displayManager.h>
#include <ts3/system/displayDriver.h>
#include <ts3/system/sysContextNative.h>
#include <ts3/system/windowNative.h>
#include <ts3/system/eventSystemNative.h>

using namespace ts3::system;

int main( int pArgc, const char ** pArgv )
{
    SysContextCreateInfo sysCtxCreateInfo;
    sysCtxCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    sysCtxCreateInfo.flags = 0;

    auto sysCtx = creCreateSystemContext( sysCtxCreateInfo );
    auto dmgr = createSysObject<DisplayManager>( sysCtx );
    auto ddrv = dmgr->createDisplayDriver( EDisplayDriverType::Generic );

    ddrv->syncDisplayConfiguration();
    auto dcdump = ddrv->generateConfigurationDump();
    printf( "%s\n", dcdump.c_str() );
    
    WindowCreateInfo wci;
    wci.properties.geometry.size = {1600,900};
    wci.properties.geometry.frameStyle = WindowFrameStyle::Caption;
    wci.properties.title = "SysTest";

    auto wmgr = createSysObject<WindowManager>( dmgr );
    auto * wnd = wmgr->createWindow( wci );

    bool runApp = true;

    auto evts = createSysObject<EventController>( sysCtx );
    auto * evtd = evts->getEventDispatcher( CX_EVENT_DISPATCHER_ID_DEFAULT );
    evts->setActiveDispatcher( *evtd );

    evtd->bindEventHandler( EEventCodeIndex::AppActivityQuit, [&runApp,&wnd](const EventObject & pEvt) -> bool {
        if( wnd )
        {
            wnd->destroy();
            wnd = nullptr;
        }
        runApp = false;
        return true;
    });
    evtd->bindEventHandler( EEventCodeIndex::WindowUpdateClose, [wnd,evtd](const EventObject & pEvt) -> bool {
        if( pEvt.eWindowUpdateClose.sourceWindow == wnd )
        {
            evtd->postEventAppQuit();
        }
        return true;
    });
    evtd->bindEventHandler( EEventCodeIndex::InputKeyboardKey, [wnd,evtd](const EventObject & pEvt) -> bool {
        if( pEvt.eInputKeyboardKey.keyCode == KeyCode::Escape )
        {
            evtd->postEventAppQuit();
        }
        else if( pEvt.eInputKeyboardKey.keyCode == KeyCode::CharF )
        {
            wnd->setFullscreenMode( true );
        }
        else if( pEvt.eInputKeyboardKey.keyCode == KeyCode::CharG )
        {
            wnd->setFullscreenMode( false );
        }
        return true;
    });

    nativeEnableWindowEventSupport( *wnd, *evts );

    while( runApp )
    {
        evts->dispatchSysEventAuto();
    }

    wmgr->reset();

    return 0;
}
