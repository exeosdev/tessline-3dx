
#include "sysContextNative.h"

namespace ts3::system
{

    SysContext::SysContext( std::unique_ptr<SysContextNativeProxy> pNativeProxy )
    : mNativeProxy( pNativeProxy.get() )
    , mNativeDataPtr( &( pNativeProxy->mNativeData ) )
    , _nativeProxy( std::move( pNativeProxy ) )
    {}

    SysContext::~SysContext() = default;

//    void _nativeSysContextInitialize( SysContext & pContext, const SysContextCreateInfo & pCreateInfo );
//    void _nativeSysContextRelease( SysContext & pContext );
//
//    SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo )
//    {
//        auto * sysContext = new SysContext();
//        _nativeSysContextInitialize( *sysContext, pCreateInfo );
//        return sysContext;
//    }
//
//    void destroySysContext( SysContextHandle pContext )
//    {
//        _nativeSysContextRelease( *pContext );
//        delete pContext;
//    }

} // namespace ts3::system
