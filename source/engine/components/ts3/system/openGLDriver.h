
#ifndef __TS3_SYSTEM_OPENGL_SYSTEM_DRIVER_H__
#define __TS3_SYSTEM_OPENGL_SYSTEM_DRIVER_H__

#include "openGLCommon.h"

namespace ts3::system
{

	ts3SysDeclareHandle( DisplayManager );
	ts3SysDeclareHandle( OpenGLDisplaySurface );
	ts3SysDeclareHandle( OpenGLRenderContext );

	/// @brief Contains parameters used to specify how a GL display surface should be created.
	struct OpenGLDisplaySurfaceCreateInfo
	{
		/// Geometry of the frame/window/layer used to accommodate the surface.
		FrameGeometry frameGeometry = CX_FRAME_GEOMETRY_DEFAULT;

		/// The title of the frame. Support (and the actual text location) depends on the platform and type of the frame.
		std::string title;

		/// Visual config for the surface. Describes primary rendering properties of the surface.
		VisualConfig visualConfig;

		/// Minimum version of the OpenGL API the surface should support. This value is Desktop-/ES-specific.
		Version minimumAPIVersion;

		///
		EOpenGLAPIClass targetAPIClass;

		/// Creation flags, describing additional surface properties.
		Bitmask<EOpenGLSurfaceCreateFlags> flags = 0;
	};

	/// @brief Contains parameters used to specify how a GL render context should be created.
	struct OpenGLRenderContextCreateInfo
	{
		/// Handle to an existing GLRenderContext used to share resources. If set to null, no sharing is done.
		OpenGLRenderContextHandle shareContext = nullptr;

		/// Target version of the API the context should support. Supported version will at least match the requested.
		Version requestedAPIVersion = CX_GL_VERSION_BEST_SUPPORTED;

		/// Selected API profile the context should *at least* support.
		EOpenGLAPIProfile contextAPIProfile = EOpenGLAPIProfile::Auto;

		/// Creation flags, describing additional context properties.
		Bitmask<EOpenGLRenderContextCreateFlags> flags = 0;
	};

	/// @brief
	class OpenGLSystemDriver : public SysObject
	{
		friend class OpenGLDisplaySurface;
		friend class OpenGLRenderContext;

	public:
		DisplayManagerHandle const mDisplayManager;

	public:
        explicit OpenGLSystemDriver( DisplayManagerHandle pDisplayManager );
        virtual ~OpenGLSystemDriver() noexcept;

		/// @brief Initializes core OpenGL state and system-level interfaces.
		/// This method also creates any additionally required
		OpenGLVersionSupportInfo initializePlatform();

		const OpenGLVersionSupportInfo & getVersionSupportInfo() const;

		/// @brief Releases temporary init state created automatically by the library. Context must be a valid GL context.
		/// Call this method after you have created actual display surface and context. It's not mandatory, (everything
		/// will be released at shutdown anyway), but on some platforms this could free some extra memory and release
		/// couple system-level interfaces.
		void releaseInitState( OpenGLRenderContext & pRenderContext );

		/// @brief Creates a system OpenGL surface (usually - a window) with a configuration matching specified requirements.
		/// @param pCreateInfo CreateInfo struct with a surface specification (VisualConfig, window geometry, etc.)
		TS3_FUNC_NO_DISCARD OpenGLDisplaySurfaceHandle createDisplaySurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo );

		/// @brief Creates a OpenGLDisplaySurface object, that wraps currently bound surface for the current thread.
		/// @param pTargetSurface Existing surface object to be used. If null, a new OpenGLDisplaySurface will be created.
		///
		/// This function creates a logical surface object without creating an actual (system-level) surface. It enables
		/// usage of the library API in environments where low-level GL stuff is created externally (a good example is
		/// the editor, where wxWidgets manages underlying GL surface/context). Important note: on some environments there
		/// might be no way to obtain all data required to create a wrapper around surface. If that is the case, this
		/// method will throw an exception with EXC_NOT_SUPPORTED code.
		/// @note
		/// >> The surface specified as pTargetSurface is just a hint and may be discarded. Always use returned handle!
		/// @note
		/// >> If no surface is currently bound, null handle is returned, regardless of the value of pTargetSurface.
		TS3_FUNC_NO_DISCARD OpenGLDisplaySurfaceHandle createDisplaySurfaceForCurrentThread();

		/// @brief Creates a system OpenGL render context with a configuration matching specified requirements.
		/// @param pSurface Surface to be used for context creation. Context can be bound to any surface compatible with this one.
		/// @param pCreateInfo CreateInfo struct with a context specification (OpenGL API version, profile, etc.)
		TS3_FUNC_NO_DISCARD OpenGLRenderContextHandle createRenderContext( OpenGLDisplaySurface & pSurface,
                                                                           const OpenGLRenderContextCreateInfo & pCreateInfo );

		/// @brief Creates a OpenGLRenderContext object, that wraps currently bound context for the current thread.
		/// @param pTargetContext Existing context object to be used. If null, a new OpenGLRenderContext will be created.
		/// This functions works similar to createDisplaySurfaceForCurrentThread(), but does that for the currently
		/// bound OpenGL render context. See description of createDisplaySurfaceForCurrentThread() for details.
		/// @note
		/// >> The context specified as pTargetContext is just a hint and may be discarded. Always use returned handle!
		/// @note
		/// >> If no context is currently bound, null handle is returned, regardless of the value of pTargetContext.
		TS3_FUNC_NO_DISCARD OpenGLRenderContextHandle createRenderContextForCurrentThread();

		/// @brief
		void resetContextBinding();

		/// @brief
		TS3_FUNC_NO_DISCARD std::vector<EDepthStencilFormat> querySupportedDepthStencilFormats( EColorFormat pColorFormat ) const;

		/// @brief
		TS3_FUNC_NO_DISCARD std::vector<EMSAAMode> querySupportedMSAAModes( EColorFormat pColorFormat,
                                                                            EDepthStencilFormat pDepthStencilFormat ) const;

		/// @brief
		TS3_FUNC_NO_DISCARD const Version & getSupportedRuntimeVersion() const;

		/// @brief
		TS3_FUNC_NO_DISCARD bool isAPIClassSupported( EOpenGLAPIClass pAPIClass ) const;

		/// @brief
		TS3_FUNC_NO_DISCARD bool isRenderContextBound() const;

	friendapi:
		/// Used by the OpenGLDisplaySurface class. Releases OS-level state of the specified surface.
		virtual void releaseSystemDisplaySurface( OpenGLDisplaySurface & pDisplaySurface ) noexcept;

		/// Used by the OpenGLRenderContext class. Releases OS-level state of the specified context.
		virtual void releaseSystemRenderContext( OpenGLRenderContext & pRenderContext ) noexcept;

	private:
		virtual OpenGLVersionSupportInfo _nativeQueryVersionSupportInfo() const noexcept;

		virtual void _nativeInitializePlatform() = 0;

		virtual void _nativeReleaseInitState() noexcept = 0;

		virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo ) = 0;

		virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurfaceForCurrentThread() = 0;

		virtual void _nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface ) = 0;

		virtual OpenGLRenderContextHandle _nativeCreateRenderContext( OpenGLDisplaySurface & pSurface,
                                                                      const OpenGLRenderContextCreateInfo & pCreateInfo ) = 0;

		virtual OpenGLRenderContextHandle _nativeCreateRenderContextForCurrentThread() = 0;

		virtual void _nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext ) = 0;

		virtual void _nativeResetContextBinding() = 0;

		virtual std::vector<EDepthStencilFormat> _nativeQuerySupportedDepthStencilFormats( EColorFormat pColorFormat ) const = 0;

		virtual std::vector<EMSAAMode> _nativeQuerySupportedMSAAModes( EColorFormat pColorFormat,
                                                                       EDepthStencilFormat pDepthStencilFormat ) const = 0;

		virtual bool _nativeIsAPIClassSupported( EOpenGLAPIClass pAPIClass ) const = 0;

		virtual bool _nativeIsRenderContextBound() const = 0;

	protected:
		OpenGLVersionSupportInfo _versionSupportInfo;
	};

	/// @brief
	class OpenGLDisplaySurface : public Frame
	{
		friend class OpenGLSystemDriver;

	public:
		OpenGLSystemDriverHandle const mGLSystemDriver;

	public:
        explicit OpenGLDisplaySurface( OpenGLSystemDriverHandle pGLSystemDriver, void * pNativeData );
        virtual ~OpenGLDisplaySurface() noexcept;

		void clearColorBuffer();

		/// @brief
		void swapBuffers();

		///
		EOpenGLAPIClass querySupportedAPIClass() const;

		/// @brief
		TS3_FUNC_NO_DISCARD FrameSize queryRenderAreaSize() const;

		/// @brief
		TS3_FUNC_NO_DISCARD bool sysValidate() const;

		/// @copybrief Frame::resizeClientArea
		virtual void resizeClientArea( const FrameSize & pSize ) override final;

        /// @copybrief Frame::resizeFrame
        virtual void resizeFrame( const FrameSize & pSize ) override final;

		/// @copybrief Frame::setFullscreenMode
		virtual void setFullscreenMode( bool pEnable ) override final;

		/// @copybrief Frame::setTitle
		virtual void setTitle( const std::string & pTitleText ) override final;

		/// @copybrief Frame::updateGeometry
		virtual void updateGeometry( const FrameGeometry & pFrameGeometry,
                                     Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

		/// @copybrief Frame::getClientAreaSize
		TS3_FUNC_NO_DISCARD virtual FrameSize getClientAreaSize() const override final;

		/// @copybrief Frame::getSize
		TS3_FUNC_NO_DISCARD virtual FrameSize getFrameSize() const override final;

		/// @copybrief Frame::isFullscreen
		TS3_FUNC_NO_DISCARD virtual bool isFullscreen() const override final;

	protected:
		virtual void onDestroySystemObjectRequested() override;

		void setInternalOwnershipFlag( bool pOwnershipFlag );

		bool hasInternalOwnershipFlag() const;

	private:
		virtual void _nativeSwapBuffers() = 0;

		virtual EOpenGLAPIClass _nativeQuerySupportedAPIClass() const noexcept = 0;

		virtual FrameSize _nativeQueryRenderAreaSize() const = 0;

		virtual bool _nativeSysValidate() const = 0;

		virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) = 0;

		virtual void _nativeSetFullscreenMode( bool pEnable ) = 0;

		virtual void _nativeSetTitle( const std::string & pTitle ) = 0;

		virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                            Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) = 0;

		virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const = 0;

		virtual bool _nativeIsFullscreen() const = 0;

	private:
		bool _internalOwnershipFlag = true;
	};

	/// @brief
	class OpenGLRenderContext : public SysObject
	{
		friend class OpenGLSystemDriver;

	public:
		OpenGLSystemDriverHandle const mGLSystemDriver;

	public:
        explicit OpenGLRenderContext( OpenGLSystemDriverHandle pGLSystemDriver );
        virtual ~OpenGLRenderContext() noexcept;

		/// @brief
		void bindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface );

		/// @brief
		TS3_FUNC_NO_DISCARD bool sysCheckIsCurrent() const;

		/// @brief
		TS3_FUNC_NO_DISCARD bool sysValidate() const;

		/// @brief
		TS3_FUNC_NO_DISCARD OpenGLSystemVersionInfo querySystemVersionInfo() const;

	protected:
		virtual void onDestroySystemObjectRequested() override;

		void setInternalOwnershipFlag( bool pOwnershipFlag );

		bool hasInternalOwnershipFlag() const;

	private:
		virtual void _nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface ) = 0;

		virtual bool _nativeSysCheckIsCurrent() const = 0;

		virtual bool _nativeSysValidate() const = 0;

	private:
		bool _internalOwnershipFlag = true;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_OPENGL_SYSTEM_DRIVER_H__
