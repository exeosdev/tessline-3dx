
#include "sysContextNative.h"

namespace ts3
{
namespace system
{

    void _nativeSysContextInitialize( SysContext & pContext, const SysContextCreateInfo & pCreateInfo );
    void _nativeSysContextRelease( SysContext & pContext );

    SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo )
    {
        auto * sysContext = new SysContext();
        _nativeSysContextInitialize( *sysContext, pCreateInfo );
        return sysContext;
    }

    void destroySysContext( SysContextHandle pContext )
    {
        _nativeSysContextRelease( *pContext );
        delete pContext;
    }

} // namespace system
} // namespace ts3
