
#ifndef __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
#define __TS3_SYSTEM_GFX_OPENGL_COMMON_H__

#include "visual.h"
#include "windowCommon.h"

/// @brief Identifies a desktop OpenGL-based platform (like Win32, GLX or MacOS).
#define TS3_SYSTEM_GL_PLATFORM_TYPE_DESKTOP 0x7001

/// @brief Identifies an ES-based platform (like Android or iOS).
#define TS3_SYSTEM_GL_PLATFORM_TYPE_ES 0x7002

/// @def TS3_SYSTEM_GL_PLATFORM_TYPE
/// @brief Defined as either TS3_SYSTEM_GL_PLATFORM_TYPE_DESKTOP or TS3_SYSTEM_GL_PLATFORM_TYPE_ES, depending on the target platform.

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_DESKTOP )
#  define TS3_SYSTEM_GL_PLATFORM_TYPE TS3_SYSTEM_GL_PLATFORM_TYPE_DESKTOP
#  include <GL/glew.h>
#else
#  define TS3_SYSTEM_GL_PLATFORM_TYPE TS3_SYSTEM_GL_PLATFORM_TYPE_ES
#  include <GLES3/gl3platform.h>
#  include <GLES3/gl3.h>
#  include <GLES3/gl31.h>
#endif

/// @def TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS
/// @brief Controls the GL error-checking macros. If TRUE, they are replaced with appropriate calls. Otherwise, all calls are no-ops.
/// @see ts3OpenGLCheckLastResult
/// @see ts3OpenGLCheckLastError
/// @see ts3OpenGLHandleLastError
/// @see ts3OpenGLResetErrorQueue
#if !defined( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  if( TS3_RELEASE_OPT_MAX )
#    define TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS 0
#  else
#    define TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS 1
#  endif
#endif

namespace ts3::system
{

	ts3SysDeclareHandle( OpenGLSystemDriver );
	ts3SysDeclareHandle( OpenGLDisplaySurface );
	ts3SysDeclareHandle( OpenGLRenderContext );

	struct OpenGLDisplaySurfaceCreateInfo;
	struct OpenGLRenderContextCreateInfo;

	/// @brief
	enum EOpenGLSurfaceCreateFlags : uint32
	{
		/// Surface is created as a fullscreen layer/window, adjusted to the selected display dimensions.
		E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT = 0x1000,

		/// Enables adaptive sync for the surface: v-sync is performed only when the frame rate exceeds vertical frequency.
		E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT = 0x2000,

		/// Disables vertical sync for the surface: buffers are swapped immediately upon request.
		E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_DISABLED_BIT = 0x4000,

		/// Enables vertical sync for the surface: swap is performed during a v-blank.
		E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT = 0x8000,
	};

	/// @brief
	enum EOpenGLRenderContextCreateFlags : uint32
	{
		E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT = 0x1000,
		E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT = 0x2000,
		E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT = 0x0010,
		E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_SHARE_WITH_CURRENT_BIT = 0x0020
	};

	enum class EOpenGLAPIClass : enum_default_value_t
	{
		OpenGLDesktop,
		OpenGLES
	};

	/// @brief
	enum class EOpenGLAPIProfile : enum_default_value_t
	{
		Auto,
		Core,
		GLES,
		Legacy,
	};

	enum : exception_code_value_t
	{
		///
		E_EXC_SYSTEM_OPENGL_API_VERSION_NOT_SUPPORTED = ts3::cxdefs::declareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, 0x03 ),
		E_EXC_SYSTEM_OPENGL_API_PROFILE_NOT_SUPPORTED = ts3::cxdefs::declareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, 0x04 ),
	};

	inline constexpr Version CX_GL_VERSION_BEST_SUPPORTED{ CX_UINT16_MAX, CX_UINT16_MAX };

	inline constexpr Version CX_GL_VERSION_UNKNOWN{ 0, 0 };

	inline constexpr Version CX_GL_VERSION_MAX_DESKTOP{ 4, 6 };

	inline constexpr Version CX_GL_VERSION_MAX_ES{ 3, 2 };

	/// @brief
	struct OpenGLVersionSupportInfo
	{
		EOpenGLAPIClass apiClass;

		EOpenGLAPIProfile apiProfile;

		Version apiVersion;
	};

	/// @brief Represents combined info about the current OpenGL subsystem version.
	/// Basically, this struct stores the output from all version-related GL queries.
	struct OpenGLSystemVersionInfo
	{
	public:
		// Numeric version of the GL (GL_VERSION_MAJOR.GL_VERSION_MINOR)
		Version apiVersion;
		// Text version of the GL (GL_VERSION)
		std::string apiVersionStr;
		// Text version of the GLSL (GL_SHADING_LANGUAGE_VERSION)
		std::string glslVersionStr;
		// Name of the renderer (GL_RENDERER_NAME)
		std::string rendererName;
		// Name of the vendor (GL_VENDOR_NAME)
		std::string vendorName;

	public:
		TS3_ATTR_NO_DISCARD	std::string toString() const;
	};

	struct OpenGLErrorInfo
	{
	public:
		// The error identifier. It will contain either a common OpenGL error
		// code (GLenum) or one of the subsystem-specific ones (AGL/EGL/GLX/WGL).
		uint32 errorCode;

		// Message describing
		const char * errorString;

	public:
		constexpr OpenGLErrorInfo( bool pStatus )
		: errorCode( pStatus ? 0u : Limits<uint32>::maxValue )
		, errorString( CX_STR_CHAR_EMPTY )
		{}

		template <typename TGLErrorCode>
		constexpr OpenGLErrorInfo( TGLErrorCode pErrorCode, const char * pErrorMessage = nullptr )
		: errorCode( numeric_cast<uint32>( pErrorCode ) )
		, errorString( pErrorMessage ? CX_STR_CHAR_EMPTY : pErrorMessage )
		{}
	};

	class GLSystemException : public SystemException
	{
	public:
		OpenGLErrorInfo mOpenGLErrorInfo;

	public:
		explicit GLSystemException( ExceptionInfo pExceptionInfo )
		: SystemException( std::move( pExceptionInfo ) )
		, mOpenGLErrorInfo( true )
		{}

		GLSystemException( ExceptionInfo pExceptionInfo, OpenGLErrorInfo pOpenGLErrorInfo )
		: SystemException( std::move( pExceptionInfo ) )
		, mOpenGLErrorInfo( std::move( pOpenGLErrorInfo ) )
		{}
	};

	ts3SetExceptionCategoryType( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, GLSystemException );

	class OpenGLCoreAPI
	{
	public:
		static Version queryRuntimeVersion();

		static bool checkLastResult();

		static bool checkLastError( GLenum pErrorCode );

		static void handleLastError();

		static void resetErrorQueue();

		static const char * translateErrorCode( GLenum pError );
	};

} // namespace ts3::system

/// @def ts3OpenGLCheckLastResult
/// @brief A configuration-dependent macro which expands to either OpenGLCoreAPI::checkLastResult() or an empty statement.
/// @see TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS
/// @see ts3::system::OpenGLCoreAPI::checkLastResult

/// @def ts3OpenGLCheckLastError
/// @brief A configuration-dependent macro which expands to either OpenGLCoreAPI::checkLastError() or an empty statement.
/// @see TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS
/// @see ts3::system::OpenGLCoreAPI::checkLastError

/// @def ts3OpenGLHandleLastError
/// @brief A configuration-dependent macro which expands to either OpenGLCoreAPI::handleLastError() or an empty statement.
/// @see TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS
/// @see ts3::system::OpenGLCoreAPI::handleLastError

/// @def ts3OpenGLResetErrorQueue
/// @brief A configuration-dependent macro which expands to either OpenGLCoreAPI::resetErrorQueue() or an empty statement.
/// @see TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS
/// @see ts3::system::OpenGLCoreAPI::resetErrorQueue()

#if( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  define ts3OpenGLCheckLastResult()            ::ts3::system::OpenGLCoreAPI::checkLastResult()
#  define ts3OpenGLCheckLastError( pErrorCode ) ::ts3::system::OpenGLCoreAPI::checkLastError( pErrorCode )
#  define ts3OpenGLHandleLastError()            ::ts3::system::OpenGLCoreAPI::handleLastError()
#  define ts3OpenGLResetErrorQueue()            ::ts3::system::OpenGLCoreAPI::resetErrorQueue()
#else
#  define ts3OpenGLCheckLastResult()
#  define ts3OpenGLCheckLastError( pErrorCode )
#  define ts3OpenGLHandleLastError()
#  define ts3OpenGLResetErrorQueue()
#endif

#endif // __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
