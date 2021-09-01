
#include <ts3/system/displayManager.h>
#include <ts3/system/displayDriver.h>
#include <ts3/system/sysContextNative.h>

int main( int pArgc, const char ** pArgv )
{
    ts3::system::SysContextCreateInfo sysCtxCreateInfo;
    sysCtxCreateInfo.nativeParams.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    sysCtxCreateInfo.flags = 0;

    auto sysCtx = ts3::system::creCreateSystemContext( sysCtxCreateInfo );
    auto dmgr = new ts3::system::DisplayManager( sysCtx );
    auto ddrv = dmgr->createDisplayDriver( ts3::system::EDisplayDriverType::Generic );

    ddrv->initializeDisplayConfiguration();
    auto dcdump = ddrv->dumpDisplayConfiguration();

    printf( "%s\n", dcdump.c_str() );

    system("pause");
    return 0;
}
