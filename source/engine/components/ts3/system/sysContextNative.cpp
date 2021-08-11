
#include "sysContextNative.h"

namespace ts3
{
namespace system
{

    SysContextNativeProxy::SysContextNativeProxy()
    : SysContext( *this, _nativeData )
    {}

    SysContextNativeProxy::~SysContextNativeProxy()
    {}

}
}
