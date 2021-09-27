
#include "sysContextNative.h"

namespace ts3::system
{

    SysContext::SysContext()
    : mInternal( std::make_unique<ContextPrivateData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {}

    SysContext::~SysContext() noexcept
    {}

} // namespace ts3::system
