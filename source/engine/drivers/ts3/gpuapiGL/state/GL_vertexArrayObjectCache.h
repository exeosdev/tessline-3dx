
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_CACHE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_CACHE_H__

#include "../GL_prerequisites.h"
#include <map>

namespace ts3::gpuapi
{

	class GLIAInputLayoutImmutableState;
	class GLIAVertexStreamImmutableState;

	ts3GLDeclareOpenGLObjectHandle( GLVertexArrayObject );

	struct GLVertexArrayObjectCachedID
	{
		uint64 inputLayoutStatePtr;
		uint64 vertexStreamStatePtr;

		bool operator==( const GLVertexArrayObjectCachedID & pRhs ) const
		{
			return ( inputLayoutStatePtr == pRhs.inputLayoutStatePtr ) && ( vertexStreamStatePtr == pRhs.vertexStreamStatePtr );
		}

		bool operator<( const GLVertexArrayObjectCachedID & pRhs ) const
		{
			return ( inputLayoutStatePtr < pRhs.inputLayoutStatePtr ) ||
			       ( ( inputLayoutStatePtr == pRhs.inputLayoutStatePtr ) && ( vertexStreamStatePtr < pRhs.vertexStreamStatePtr ) );
		}
	};

	class GLVertexArrayObjectCache
	{
	public:
		GLVertexArrayObjectCache();
		~GLVertexArrayObjectCache();

		const GLVertexArrayObject & getOrCreate(
				const GLIAInputLayoutImmutableState & pInputLayoutState,
				const GLIAVertexStreamImmutableState & pVertexStreamState );

		void reset();

	private:
		using VertexArrayObjectMap = std::map<GLVertexArrayObjectCachedID, GLVertexArrayObjectHandle>;
		VertexArrayObjectMap _vertexArrayObjectMap;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_CACHE_H__
