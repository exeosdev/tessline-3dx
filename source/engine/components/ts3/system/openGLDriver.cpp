
#include <ts3/system/internal/openGLPrivate.h>
#include "displaySystem.h"

namespace ts3::system
{

    OpenGLSystemDriver::OpenGLSystemDriver( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( std::move( pDisplayManager ) )
    , _privateData( std::make_unique<OpenGLSystemDriverPrivateData>() )
    {}

    OpenGLSystemDriver::~OpenGLSystemDriver() noexcept = default;

    void OpenGLSystemDriver::initializePlatform()
    {
        _nativeInitializePlatform();

        _privateData->supportedRuntimeVersion = GLCoreAPI::queryRuntimeVersion();

        if( _privateData->supportedRuntimeVersion.major == 0 )
        {
            _privateData->supportedRuntimeVersion.major = 1;
            _privateData->supportedRuntimeVersion.minor = 0;
        }
    }

    void OpenGLSystemDriver::releaseInitState( OpenGLRenderContext & /* pGLRenderContext */ )
    {
        _nativeReleaseInitState();
    }

    OpenGLDisplaySurfaceHandle OpenGLSystemDriver::createDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo )
    {
        GLDisplaySurfaceCreateInfo surfaceCreateInfo = pCreateInfo;

        if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
        {
            surfaceCreateInfo.frameGeometry.size = CX_FRAME_SIZE_MAX;
            surfaceCreateInfo.frameGeometry.style = EFrameStyle::Overlay;
        }
        else
        {
            surfaceCreateInfo.frameGeometry.position = pCreateInfo.frameGeometry.position;
            surfaceCreateInfo.frameGeometry.size = pCreateInfo.frameGeometry.size;
            surfaceCreateInfo.frameGeometry.style = pCreateInfo.frameGeometry.style;
        }

        surfaceCreateInfo.frameGeometry = mDisplayManager->validateFrameGeometry( surfaceCreateInfo.frameGeometry );

        auto displaySurface = _nativeCreateDisplaySurface( surfaceCreateInfo );
        displaySurface->setInternalOwnershipFlag( true );

        return displaySurface;
    }

    OpenGLDisplaySurfaceHandle OpenGLSystemDriver::createDisplaySurfaceForCurrentThread()
    {
        auto displaySurface = _nativeCreateDisplaySurfaceForCurrentThread();
        displaySurface->setInternalOwnershipFlag( false );

        return displaySurface;
    }

    OpenGLRenderContextHandle OpenGLSystemDriver::createRenderContext( OpenGLDisplaySurface & pSurface,
                                                                       const GLRenderContextCreateInfo & pCreateInfo )
    {
        GLRenderContextCreateInfo surfaceCreateInfo = pCreateInfo;

        if( surfaceCreateInfo.runtimeVersionDesc.apiVersion == cvVersionUnknown  )
        {
            surfaceCreateInfo.runtimeVersionDesc.apiVersion = Version{ 1, 0 };
        }
        else if( surfaceCreateInfo.runtimeVersionDesc.apiVersion == CX_GL_VERSION_BEST_SUPPORTED )
        {
            surfaceCreateInfo.runtimeVersionDesc.apiVersion = _privateData->supportedRuntimeVersion;
        }

        if( surfaceCreateInfo.runtimeVersionDesc.apiProfile == EGLAPIProfile::OpenGLES )
        {
            if( surfaceCreateInfo.runtimeVersionDesc.apiVersion > CX_GL_VERSION_MAX_ES )
            {
                return nullptr;
            }
        }
        else
        {
            if( surfaceCreateInfo.runtimeVersionDesc.apiVersion > CX_GL_VERSION_MAX_DESKTOP )
            {
                return nullptr;
            }
        }

        if( surfaceCreateInfo.runtimeVersionDesc.apiVersion == Version{ 3, 1 } )
        {
            if( ( surfaceCreateInfo.contextProfile == EGLContextProfile::Auto ) || ( surfaceCreateInfo.contextProfile == EGLContextProfile::Core ) )
            {
                surfaceCreateInfo.flags.set( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );
            }
            else
            {
                surfaceCreateInfo.flags.unset( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );
            }
        }

        if( surfaceCreateInfo.runtimeVersionDesc.apiVersion >= Version{ 3, 2 } )
        {
            if( surfaceCreateInfo.contextProfile == EGLContextProfile::Auto )
            {
                surfaceCreateInfo.contextProfile = EGLContextProfile::Core;
            }
        }

        auto renderContext = _nativeCreateRenderContext( pSurface, surfaceCreateInfo );
        renderContext->setInternalOwnershipFlag( true );

        return renderContext;
    }

    OpenGLRenderContextHandle OpenGLSystemDriver::createRenderContextForCurrentThread()
    {
        auto renderContext = _nativeCreateRenderContextForCurrentThread();
        renderContext->setInternalOwnershipFlag( false );

        return renderContext;
    }

    std::vector<EDepthStencilFormat> OpenGLSystemDriver::querySupportedDepthStencilFormats( EColorFormat pColorFormat ) const
    {
        return _nativeQuerySupportedDepthStencilFormats( pColorFormat);
    }

    std::vector<EMSAAMode> OpenGLSystemDriver::querySupportedMSAAModes( EColorFormat pColorFormat,
                                                                        EDepthStencilFormat pDepthStencilFormat ) const
    {
        return _nativeQuerySupportedMSAAModes( pColorFormat, pDepthStencilFormat );
    }

    void OpenGLSystemDriver::resetContextBinding()
    {
        _nativeResetContextBinding();
    }

    bool OpenGLSystemDriver::isRenderContextBound() const
    {
        return _nativeIsRenderContextBound();
    }

    void OpenGLSystemDriver::onDisplaySurfaceDestroy( OpenGLDisplaySurface & pDisplaySurface ) noexcept
    {
//        try
//        {
//            if( pDisplaySurface.mInternal->internalOwnershipFlag && _nativeIsDisplaySurfaceValid( pDisplaySurface ) )
//            {
//                _nativeDestroyDisplaySurface( pDisplaySurface );
//                pDisplaySurface.mInternal->internalOwnershipFlag = false;
//            }
//
//            auto surfaceIter = mInternal->internalDisplaySurfaceList.begin();
//            while( surfaceIter != mInternal->internalDisplaySurfaceList.end() )
//            {
//                if( *surfaceIter == &pDisplaySurface )
//                {
//                    mInternal->internalDisplaySurfaceList.erase( surfaceIter );
//                    break;
//                }
//                ++surfaceIter;
//            }
//        }
//        catch( const Exception & pException )
//        {
//            ( pException );
//            ts3DebugInterrupt();
//        }
//        catch( ... )
//        {
//            ts3DebugInterrupt();
//        }
    }

    void OpenGLSystemDriver::onRenderContextDestroy( OpenGLRenderContext & pRenderContext ) noexcept
    {
//        try
//        {
//            if( pRenderContext.mInternal->internalOwnershipFlag && _nativeIsRenderContextValid( pRenderContext ) )
//            {
//                _nativeDestroyRenderContext( pRenderContext );
//                pRenderContext.mInternal->internalOwnershipFlag = false;
//            }
//
//            auto contextIter = mInternal->internalRenderContextList.begin();
//            while( contextIter != mInternal->internalRenderContextList.end() )
//            {
//                if( *contextIter == &pRenderContext )
//                {
//                    mInternal->internalRenderContextList.erase( contextIter );
//                    break;
//                }
//                ++contextIter;
//            }
//        }
//        catch( const Exception & pException )
//        {
//            ( pException );
//            ts3DebugInterrupt();
//        }
//        catch( ... )
//        {
//            ts3DebugInterrupt();
//        }
    }


    OpenGLDisplaySurface::OpenGLDisplaySurface( OpenGLSystemDriverHandle pGLSystemDriver, void * pNativeData )
    : Frame( pGLSystemDriver->mSysContext )
    , mGLSystemDriver( std::move( pGLSystemDriver ) )
    , _privateData( std::make_unique<OpenGLDisplaySurfacePrivateData>() )
    {
        setEventSourceNativeData( pNativeData, {} );
    }

    OpenGLDisplaySurface::~OpenGLDisplaySurface() noexcept
    {
        resetEventSourceNativeData();
    }

    void OpenGLDisplaySurface::swapBuffers()
    {
        _nativeSwapBuffers();
    }

    FrameSize OpenGLDisplaySurface::queryRenderAreaSize() const
    {
        return _nativeQueryRenderAreaSize();
    }

    bool OpenGLDisplaySurface::isValid() const
    {
        return _nativeIsValid();
    }

    void OpenGLDisplaySurface::resize( const FrameSize & pSize )
    {
        FrameGeometry newFrameGeometry;
        newFrameGeometry.position = CX_FRAME_POS_AUTO;
        newFrameGeometry.size = pSize;
        newFrameGeometry.style = EFrameStyle::Unspecified;

        newFrameGeometry = mGLSystemDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

        const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_OUTER_RECT_BIT;
        _nativeUpdateGeometry( newFrameGeometry, updateFlags );
    }

    void OpenGLDisplaySurface::resizeClientArea( const FrameSize & pSize )
    {
        FrameGeometry newFrameGeometry;
        newFrameGeometry.position = CX_FRAME_POS_AUTO;
        newFrameGeometry.size = pSize;
        newFrameGeometry.style = EFrameStyle::Unspecified;

        newFrameGeometry = mGLSystemDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

        const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_CLIENT_AREA_BIT;
        _nativeUpdateGeometry( newFrameGeometry, updateFlags );
    }

    void OpenGLDisplaySurface::setFullscreenMode( bool pEnable )
    {
        _nativeSetFullscreenMode( pEnable );
    }

    void OpenGLDisplaySurface::setTitle( const std::string & pTitleText )
    {
        _nativeSetTitle( pTitleText );
    }

    void OpenGLDisplaySurface::updateGeometry( const FrameGeometry & pFrameGeometry,
                                               Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
    {
        _nativeUpdateGeometry( pFrameGeometry, pUpdateFlags );
    }

    FrameSize OpenGLDisplaySurface::getClientAreaSize() const
    {
        return _nativeGetSize( EFrameSizeMode::ClientArea );
    }

    FrameSize OpenGLDisplaySurface::getSize() const
    {
        return _nativeGetSize( EFrameSizeMode::OuterRect );
    }

    bool OpenGLDisplaySurface::isFullscreen() const
    {
        return false;
    }

    void OpenGLDisplaySurface::setInternalOwnershipFlag( bool pOwnershipFlag )
    {
        _privateData->internalOwnershipFlag = pOwnershipFlag;
    }


    OpenGLRenderContext::OpenGLRenderContext( OpenGLSystemDriverHandle pDriver )
    : SysObject( pDriver->mSysContext )
    , mGLSystemDriver( std::move( pDriver ) )
    , _privateData( std::make_unique<OpenGLRenderContextPrivateData>() )
    {}

    OpenGLRenderContext::~OpenGLRenderContext() noexcept = default;

    void OpenGLRenderContext::bindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface )
    {
        _nativeBindForCurrentThread( pTargetSurface );
    }

    bool OpenGLRenderContext::isCurrent() const
    {
        return _nativeIsCurrent();
    }

    bool OpenGLRenderContext::isValid() const
    {
        return _nativeIsValid();
    }

    GLSystemVersionInfo OpenGLRenderContext::querySystemVersionInfo() const
    {
        if( !isValid() )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        GLSystemVersionInfo systemVersionInfo;
        systemVersionInfo.apiVersion = GLCoreAPI::queryRuntimeVersion();

        if ( const auto * versionStr = glGetString( GL_VERSION ) )
        {
            systemVersionInfo.apiVersionStr.assign( reinterpret_cast<const char *>( versionStr ) );
        }
        if ( const auto * glslVersionStr = glGetString( GL_SHADING_LANGUAGE_VERSION ) )
        {
            systemVersionInfo.glslVersionStr.assign( reinterpret_cast<const char *>( glslVersionStr ) );
        }
        if ( const auto * rendererNameStr = glGetString( GL_RENDERER ) )
        {
            systemVersionInfo.rendererName.assign( reinterpret_cast<const char *>( rendererNameStr ) );
        }
        if ( const auto * vendorNameStr = glGetString( GL_VENDOR ) )
        {
            systemVersionInfo.vendorName.assign( reinterpret_cast<const char *>( vendorNameStr ) );
        }

        ts3GLResetErrorQueue();

        return systemVersionInfo;
    }

    void OpenGLRenderContext::setInternalOwnershipFlag( bool pOwnershipFlag )
    {
        _privateData->internalOwnershipFlag = pOwnershipFlag;
    }

} // namespace ts3::system
