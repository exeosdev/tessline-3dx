
#include "GL_vertexArrayObjectCache.h"
#include "../objects/GL_vertexArrayObject.h"
#include "../state/GL_inputAssembler.h"

namespace ts3::gpuapi
{

	GLVertexArrayObjectCache::GLVertexArrayObjectCache() = default;

	GLVertexArrayObjectCache::~GLVertexArrayObjectCache() = default;

	const GLVertexArrayObject & GLVertexArrayObjectCache::getOrCreate(
			const GLIAInputLayoutImmutableState & pInputLayoutState,
			const GLIAVertexStreamImmutableState & pVertexStreamState )
	{
		GLVertexArrayObjectCachedID cachedID{
			reinterpret_cast<uint64>( &pInputLayoutState ),
			reinterpret_cast<uint64>( &pVertexStreamState )
		};

		auto cachedEntryIter = _vertexArrayObjectMap.find( cachedID );
		if( cachedEntryIter == _vertexArrayObjectMap.end() )
		{
			auto vertexArrayObject = smutil::createGLVertexArrayObjectLayoutStreamCombined(
				pInputLayoutState.mGLInputLayoutDefinition,
				pVertexStreamState.mGLVertexStreamDefinition );

			auto insertResult = _vertexArrayObjectMap.emplace( cachedID, std::move( vertexArrayObject ) );

			cachedEntryIter = insertResult.first;
		}

		return *cachedEntryIter->second;
	}

	void GLVertexArrayObjectCache::reset()
	{
		_vertexArrayObjectMap.clear();
	}

}
