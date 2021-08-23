
#include "openGLNative.h"

namespace ts3
{
namespace system
{

    GLSystemDriverNativeImpl::GLSystemDriverNativeImpl(  DisplayManagerHandle pDisplayManager  )
    : GLSystemDriver( std::move( pDisplayManager ) )
    {}

    GLSystemDriverNativeImpl::~GLSystemDriverNativeImpl() = default;


    GLDisplaySurfaceNativeImpl::GLDisplaySurfaceNativeImpl( GLSystemDriverHandle pDriver )
    : GLDisplaySurface( std::move( pDriver ) )
    {}

    GLDisplaySurfaceNativeImpl::~GLDisplaySurfaceNativeImpl() = default;


    GLRenderContextNativeImpl::GLRenderContextNativeImpl( GLSystemDriverHandle pDriver )
    : GLRenderContext( std::move( pDriver ) )
    {}

    GLRenderContextNativeImpl::~GLRenderContextNativeImpl() = default;

}
}
