
#include <ts3/system/displayManager.h>
#include <ts3/system/displayDriver.h>
#include <ts3/system/sysContextNative.h>
#include <ts3/system/window.h>

using namespace ts3::system;

int main( int pArgc, const char ** pArgv )
{
    SysContextCreateInfo sysCtxCreateInfo;
    sysCtxCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    sysCtxCreateInfo.flags = 0;

    auto sysCtx = creCreateSystemContext( sysCtxCreateInfo );
    auto dmgr = createSysObject<DisplayManager>( sysCtx );
    auto ddrv = dmgr->createDisplayDriver( EDisplayDriverType::Generic );

    ddrv->initializeDisplayConfiguration();
    auto dcdump = ddrv->generateConfigurationDump();
    printf( "%s\n", dcdump.c_str() );
    
    WindowCreateInfo wci;
    wci.properties.geometry.size = {1600,900};
    wci.properties.geometry.frameStyle = WindowFrameStyle::Caption;
    wci.properties.title = "SysTest";

    auto wmgr = createSysObject<WindowManager>( dmgr );
    auto * wnd = wmgr->createWindow( wci );

    system("pause");
    return 0;
}
