
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_CACHE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_CACHE_H__

#include "../GL_prerequisites.h"
#include <map>

namespace ts3::gpuapi
{

	struct GLIAInputLayoutDefinition;
	struct GLIAVertexStreamDefinition;

	class GLIAInputLayoutImmutableStateCompat;
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
				const GLIAInputLayoutImmutableStateCompat & pInputLayoutState,
				const GLIAVertexStreamImmutableState & pVertexStreamState );

		const GLVertexArrayObject & getOrCreate(
				const GLIAInputLayoutDefinition & pInputLayoutDefinition,
				const GLIAVertexStreamDefinition & pVertexStreamDefinition );

		void reset();

	private:
		using PersistentVertexArrayObjectMap = std::map<GLVertexArrayObjectCachedID, GLVertexArrayObjectHandle>;
		PersistentVertexArrayObjectMap _persistentVertexArrayObjectMap;

		using TransientVertexArrayObjectMap = std::map<UniqueGPUObjectID, GLVertexArrayObjectHandle>;
		TransientVertexArrayObjectMap _transientVertexArrayObjectMap;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_CACHE_H__
