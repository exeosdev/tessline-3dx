
#include "GL_samplerObject.h"

namespace ts3::gpuapi
{

	GLSamplerObject::GLSamplerObject( GLuint pHandle )
	: GLObject( GLObjectBaseType::Sampler, pHandle )
	{}

	GLSamplerObject::~GLSamplerObject() = default;

	GLSamplerObjectHandle GLSamplerObject::create( const GLSamplerState & pSamplerState )
	{
		GLuint samplerHandle = 0;

		glGenSamplers( 1, &samplerHandle );
		ts3OpenGLHandleLastError();

		GLSamplerObjectHandle openglSamplerObject{ new GLSamplerObject( samplerHandle ) };
		if( !openglSamplerObject->setSamplerState( pSamplerState ) )
		{
			return nullptr;
		}

		return openglSamplerObject;
	}

	bool GLSamplerObject::release()
	{
		glDeleteSamplers( 1, &mGLHandle );
		ts3OpenGLHandleLastError();

		return true;
	}

	bool GLSamplerObject::validateHandle() const
	{
		GLboolean checkResult = glIsSampler( mGLHandle );
		return checkResult == GL_TRUE;
	}

	bool GLSamplerObject::setSamplerState( const GLSamplerState & pSamplerState )
	{
		glSamplerParameteri( mGLHandle, GL_TEXTURE_WRAP_S, pSamplerState.addressModeS );
		ts3OpenGLCheckLastResult();

		glSamplerParameteri( mGLHandle, GL_TEXTURE_WRAP_T, pSamplerState.addressModeT );
		ts3OpenGLCheckLastResult();

		glSamplerParameteri( mGLHandle, GL_TEXTURE_WRAP_R, pSamplerState.addressModeR );
		ts3OpenGLCheckLastResult();

		glSamplerParameteri( mGLHandle, GL_TEXTURE_MAG_FILTER, pSamplerState.magFilter );
		ts3OpenGLCheckLastResult();

		glSamplerParameteri( mGLHandle, GL_TEXTURE_MIN_FILTER, pSamplerState.minFilter );
		ts3OpenGLCheckLastResult();

	#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_ANISOTROPIC_FILTER )
		if( pSamplerState.anisotropyLevel > 0 )
		{
			glSamplerParameteri( mGLHandle, GL_TEXTURE_MAX_ANISOTROPY, pSamplerState.anisotropyLevel );
			ts3OpenGLCheckLastResult();
		}
	#endif

		glSamplerParameterf( mGLHandle, GL_TEXTURE_MIN_LOD, pSamplerState.mipLODRange.first );
		ts3OpenGLCheckLastResult();

		glSamplerParameterf( mGLHandle, GL_TEXTURE_MAX_LOD, pSamplerState.mipLODRange.second );
		ts3OpenGLCheckLastResult();

	#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_EXTENDED_ADDRESS_MODE )
		glSamplerParameterfv( mGLHandle, GL_TEXTURE_BORDER_COLOR, &( pSamplerState.borderColor.rgbaArray[0] ) );
		ts3OpenGLCheckLastResult();
	#endif

		glSamplerParameteri( mGLHandle, GL_TEXTURE_COMPARE_MODE, pSamplerState.textureCompareMode );
		ts3OpenGLCheckLastResult();

		if( pSamplerState.textureCompareMode == GL_COMPARE_REF_TO_TEXTURE )
		{
			glSamplerParameteri( mGLHandle, GL_TEXTURE_COMPARE_FUNC, pSamplerState.textureCompareFunc );
			ts3OpenGLCheckLastResult();
		}

		return true;
	}

} // namespace ts3::gpuapi
