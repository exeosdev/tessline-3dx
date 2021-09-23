
#include "sysContextNative.h"

namespace ts3::system
{

    SysContext::SysContext()
    : mInternal( std::make_unique<ContextPrivateData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        _nativeInitialize();
    }

    SysContext::~SysContext()
    {
        _nativeRelease();
    }


    SysContextHandle creCreateSystemContext( const SysContextCreateInfo & pCreateInfo )
    {
        auto context = std::make_shared<SysContext>();
        return context;
    }

    void creDestroySysContext( SysContextHandle pContext )
    {
        ( pContext );
    }

} // namespace ts3::system
