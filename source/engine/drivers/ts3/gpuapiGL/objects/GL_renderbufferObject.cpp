
#include "GL_renderbufferObject.h"

namespace ts3::gpuapi
{

	GLRenderbufferObject::GLRenderbufferObject( GLuint pHandle, const GLRenderbufferCreateInfo & pGLCreateInfo )
	: GLObject( GLObjectBaseType::Renderbuffer, pHandle )
	, mDimensions( pGLCreateInfo.dimensions )
	, mGLInternalFormat( pGLCreateInfo.internalFormat )
	{}

	GLRenderbufferObject::~GLRenderbufferObject() = default;

	GLRenderbufferObjectHandle GLRenderbufferObject::create( const GLRenderbufferCreateInfo & pGLCreateInfo )
	{
		GLuint renderbufferHandle = 0;

		glGenRenderbuffers( 1, &renderbufferHandle );
		ts3OpenGLHandleLastError();

		glBindRenderbuffer( GL_RENDERBUFFER, renderbufferHandle );
		ts3OpenGLHandleLastError();

		GLRenderbufferObjectHandle openglRenderbufferObject{ new GLRenderbufferObject( renderbufferHandle, pGLCreateInfo ) };
		if( !openglRenderbufferObject->initialize( pGLCreateInfo ) )
		{
			return nullptr;
		}

		return openglRenderbufferObject;
	}

	bool GLRenderbufferObject::release()
	{
		glDeleteRenderbuffers( 1, &mGLHandle );
		ts3OpenGLHandleLastError();

		return true;
	}

	bool GLRenderbufferObject::validateHandle() const
	{
		auto isBuffer = glIsRenderbuffer( mGLHandle );
		ts3OpenGLHandleLastError();

		return isBuffer != GL_FALSE;
	}

	bool GLRenderbufferObject::initialize( const GLRenderbufferCreateInfo & pGLCreateInfo )
	{
		if( pGLCreateInfo.msaaLevel == 0 )
		{
			glRenderbufferStorage( GL_RENDERBUFFER,
			                       pGLCreateInfo.internalFormat,
			                       pGLCreateInfo.dimensions.x,
			                       pGLCreateInfo.dimensions.y );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glRenderbufferStorageMultisample( GL_RENDERBUFFER,
			                                  pGLCreateInfo.msaaLevel,
			                                  pGLCreateInfo.internalFormat,
			                                  pGLCreateInfo.dimensions.x,
			                                  pGLCreateInfo.dimensions.y );
			ts3OpenGLHandleLastError();
		}

		return true;
	}

} // namespace ts3::gpuapi
