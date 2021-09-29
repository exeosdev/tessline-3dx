
#include "sysObject.h"

namespace ts3::system
{

    SysObject::SysObject( system::SysContextHandle pSysContext )
    : mSysContext( std::move( pSysContext ) )
    {}

    SysObject::~SysObject() = default;

} // namespace ts3::system
