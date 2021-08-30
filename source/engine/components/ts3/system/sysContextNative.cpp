
#include "sysContextNative.h"

namespace ts3::system
{

    SysContextNativeProxy::SysContextNativeProxy()
    : SysContext( *this, _nativeData )
    {}

    SysContextNativeProxy::~SysContextNativeProxy()
    {}

} // namespace ts3::system
