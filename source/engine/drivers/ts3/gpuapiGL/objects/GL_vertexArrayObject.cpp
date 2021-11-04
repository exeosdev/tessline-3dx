
#include "GL_vertexArrayObject.h"

namespace ts3
{
namespace gpuapi
{

	GLVertexArrayObject::GLVertexArrayObject( GLuint pHandle )
	: GLObject( GLObjectBaseType::VertexArray, pHandle )
	{}

	GLVertexArrayObject::~GLVertexArrayObject() = default;

	GLVertexArrayObjectHandle GLVertexArrayObject::create()
	{
		GLuint vertexArrayHandle = 0;

		glGenVertexArrays( 1, &vertexArrayHandle );
		ts3GLHandleLastError();

		GLVertexArrayObjectHandle openglVertexArrayObject{ new GLVertexArrayObject( vertexArrayHandle ) };

		return openglVertexArrayObject;
	}

	bool GLVertexArrayObject::release()
	{
		glDeleteVertexArrays( 1, &mGLHandle );
		ts3GLHandleLastError();

		return true;
	}

	bool GLVertexArrayObject::validateHandle() const
	{
		auto isProgram = glIsVertexArray( mGLHandle );
		ts3GLHandleLastError();

		return isProgram != GL_FALSE;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
