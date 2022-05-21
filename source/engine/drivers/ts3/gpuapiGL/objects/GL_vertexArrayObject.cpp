
#include "GL_vertexArrayObject.h"

namespace ts3::gpuapi
{

	GLVertexArrayObject::GLVertexArrayObject( GLuint pHandle )
	: GLObject( GLObjectBaseType::VertexArray, pHandle )
	{}

	GLVertexArrayObject::~GLVertexArrayObject() = default;

	GLVertexArrayObjectHandle GLVertexArrayObject::create()
	{
		GLuint vertexArrayHandle = 0;

		glGenVertexArrays( 1, &vertexArrayHandle );
		ts3OpenGLHandleLastError();

		return createGLObject<GLVertexArrayObject>( vertexArrayHandle );
	}

	bool GLVertexArrayObject::release()
	{
		glDeleteVertexArrays( 1, &mGLHandle );
		ts3OpenGLHandleLastError();

		return true;
	}

	bool GLVertexArrayObject::validateHandle() const
	{
		auto isProgram = glIsVertexArray( mGLHandle );
		ts3OpenGLHandleLastError();

		return isProgram != GL_FALSE;
	}

} // namespace ts3::gpuapi
