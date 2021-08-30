
#include "sysContextNative.h"

namespace ts3::system
{

    SysContext::SysContext()
    : mPrivateData( std::make_unique<ContextPrivateData>() )
    , mNativeData( &( mPrivateData->nativeDataPriv ) )
    {}

    SysContext::~SysContext() = default;

} // namespace ts3::system
