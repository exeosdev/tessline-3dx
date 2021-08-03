
#include "coreSessionContextNative.h"

namespace ts3
{
namespace system
{

    void _nativeCoreSessionContextInitialize( CoreSessionContext & pContext, const CoreSessionContextCreateInfo & pCreateInfo );
    void _nativeCoreSessionContextRelease( CoreSessionContext & pContext );

    CoreSessionContextHandle createCoreSessionContext( const CoreSessionContextCreateInfo & pCreateInfo )
    {
        auto * csContext = new CoreSessionContext();
        _nativeCoreSessionContextInitialize( *csContext, pCreateInfo );
        return csContext;
    }

    void destroyCoreSessionContext( CoreSessionContextHandle pContext )
    {
        _nativeCoreSessionContextRelease( *pContext );
        delete pContext;
    }

} // namespace system
} // namespace ts3
