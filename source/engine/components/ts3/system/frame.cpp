
#include "frame.h"

namespace ts3::system
{

    Frame::Frame( SysContextHandle pSysContext )
    : EventSource( std::move( pSysContext ) )
    {}

    Frame::~Frame() noexcept = default;

} // namespace ts3::system
