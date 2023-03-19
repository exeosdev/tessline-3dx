
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

} // namespace ts3::gpuapi
