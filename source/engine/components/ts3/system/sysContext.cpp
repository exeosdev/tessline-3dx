
#include "sysContextNative.h"

namespace ts3::system
{

    SysContext::SysContext()
    : mPrivate( std::make_unique<ContextPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
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
