
#ifndef __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
#define __TS3_SYSTEM_GFX_OPENGL_COMMON_H__

#include "visual.h"
#include "windowCommon.h"

#define TS3_SYSTEM_GL_PLATFORM_TYPE_CORE 0x7001
#define TS3_SYSTEM_GL_PLATFORM_TYPE_ES   0x7002

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_DESKTOP )
#  define TS3_SYSTEM_GL_PLATFORM_TYPE TS3_SYSTEM_GL_PLATFORM_TYPE_CORE
#  include <GL/glew.h>
#else
#  define TS3_SYSTEM_GL_PLATFORM_TYPE TS3_SYSTEM_GL_PLATFORM_TYPE_ES
#  include <GLES3/gl3platform.h>
#  include <GLES3/gl3.h>
#  include <GLES3/gl31.h>
#endif

#if !defined( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  if( TS3_DEBUG || TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS_NON_DEBUG )
#    define TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS 1
#  else
#    define TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS 1
#  endif
#endif

namespace ts3::system
{

	ts3SysDeclareHandle( OpenGLSystemDriver );
	ts3SysDeclareHandle( OpenGLDisplaySurface );
	ts3SysDeclareHandle( OpenGLRenderContext );

	struct GLDisplaySurfaceCreateInfo;
	struct GLRenderContextCreateInfo;

	/// @brief
	enum EGLSurfaceCreateFlags : uint32
	{
		// Specifies a full-screen surface, covering the whole screen.
		// Ignored for mobile (iOS and Android), where fullscreen is always used.
		E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT = 0x1000,
		E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT = 0x2000,
		E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_DISABLED_BIT = 0x4000,
		E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT = 0x8000,
	};

	/// @brief
	enum EGLRenderContextCreateFlags : uint32
	{
		E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT = 0x1000,
		E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT = 0x2000,
		E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT = 0x0010,
		E_GL_RENDER_CONTEXT_CREATE_FLAG_SHARE_WITH_CURRENT_BIT = 0x0020
	};

	enum class EGLAPIProfile : enum_default_value_t
	{
		OpenGL,
		OpenGLES
	};

	/// @brief
	enum class EGLContextProfile : enum_default_value_t
	{
		Auto,
		Core,
		GLES,
		Legacy,
	};

	enum : exception_code_value_t
	{
		E_EXC_SYSTEM_OPENGL_CORE_ERROR	     = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, 0xE0 ),
		E_EXC_SYSTEM_OPENGL_SUBSYS_AGL_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, 0xE1 ),
		E_EXC_SYSTEM_OPENGL_SUBSYS_EGL_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, 0xE2 ),
		E_EXC_SYSTEM_OPENGL_SUBSYS_GLX_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, 0xE3 ),
		E_EXC_SYSTEM_OPENGL_SUBSYS_WGL_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, 0xE4 )
	};

	inline constexpr Version CX_GL_VERSION_BEST_SUPPORTED{ CX_UINT16_MAX, CX_UINT16_MAX };

	inline constexpr Version CX_GL_VERSION_UNKNOWN{ 0, 0 };

	inline constexpr Version CX_GL_VERSION_MAX_DESKTOP{ 4, 6 };

	inline constexpr Version CX_GL_VERSION_MAX_ES{ 3, 2 };

	/// @brief Represents combined info about the current OpenGL subsystem version.
	/// Basically, this struct stores the output from all version-related GL queries.
	struct GLSystemVersionInfo
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
		TS3_FUNC_NO_DISCARD	std::string toString() const;
	};

	struct GLErrorInfo
	{
	public:
		// The error code. It will contains either a common OpenGL error
		// code (GLenum) or one of the subsystem-specific ones (AGL/EGL/GLX/WGL).
		uint32 errorCode;

		// Message describing
		const char * errorString;

	public:
		constexpr GLErrorInfo( bool pStatus )
		: errorCode( pStatus ? 0u : Limits<uint32>::maxValue )
		, errorString( CX_STR_CHAR_EMPTY )
		{}

		template <typename TpGLErrorCode>
		constexpr GLErrorInfo( TpGLErrorCode pErrorCode, const char * pErrorMessage = nullptr )
		: errorCode( trunc_numeric_cast<uint32>( pErrorCode ) )
		, errorString( pErrorMessage ? CX_STR_CHAR_EMPTY : pErrorMessage )
		{}
	};

	class GLSystemException : public SystemException
	{
	public:
		GLErrorInfo mGLErrorInfo;

	public:
		explicit GLSystemException( ExceptionInfo pExceptionInfo )
		: SystemException( std::move( pExceptionInfo ) )
		, mGLErrorInfo( true )
		{}

		GLSystemException( ExceptionInfo pExceptionInfo, GLErrorInfo pGLErrorInfo )
		: SystemException( std::move( pExceptionInfo ) )
		, mGLErrorInfo( std::move( pGLErrorInfo ) )
		{}
	};

	ts3SetExceptionCategoryType( E_EXCEPTION_CATEGORY_SYSTEM_OPENGL, GLSystemException );

	class GLCoreAPI
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

#if( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  define ts3GLCheckLastResult()             ::ts3::system::GLCoreAPI::checkLastResult()
#  define ts3GLCheckLastError( pErrorCode )  ::ts3::system::GLCoreAPI::checkLastError( pErrorCode )
#  define ts3GLHandleLastError()             ::ts3::system::GLCoreAPI::handleLastError()
#  define ts3GLResetErrorQueue()             ::ts3::system::GLCoreAPI::resetErrorQueue()
#else
#  define ts3GLCheckLastResult()
#  define ts3GLCheckLastError( pErrorCode )
#  define ts3GLHandleLastError()
#  define ts3GLResetErrorQueue()
#endif

#endif // __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
