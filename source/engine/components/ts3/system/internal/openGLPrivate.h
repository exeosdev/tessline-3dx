
#ifndef __TS3_SYSTEM_INTERNAL_EVENT_SYSTEM_PRIVATE_H__
#define __TS3_SYSTEM_INTERNAL_EVENT_SYSTEM_PRIVATE_H__

#include <ts3/system/openGLDriver.h>

namespace ts3::system
{

    struct OpenGLSystemDriver::OpenGLSystemDriverPrivateData
    {
        Version supportedRuntimeVersion;
    };

    struct OpenGLDisplaySurface::OpenGLDisplaySurfacePrivateData
    {
        bool internalOwnershipFlag = true;
    };

    struct OpenGLRenderContext::OpenGLRenderContextPrivateData
    {
        bool internalOwnershipFlag = true;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_INTERNAL_EVENT_SYSTEM_PRIVATE_H__
