
#include "../GL_prerequisites.h"

namespace ts3::gpuapi
{

	GLObject::GLObject( GLObjectBaseType pBaseType, GLuint pHandle )
	: mBaseType( pBaseType )
	, mGLHandle( pHandle )
	{}

	GLObject::~GLObject() = default;

	bool GLObject::release()
	{
		return true;
	}

	bool GLObject::validateHandle() const
	{
		return false;
	}

	void GLObject::invalidateHandle()
	{
		auto & writableHandle = const_cast<GLuint &>( mGLHandle );
		writableHandle = CX_GL_OBJECT_HANDLE_INVALID;
	}

} // namespace ts3::gpuapi
