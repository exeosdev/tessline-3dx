
#include "GL_vertexArrayObjectCache.h"
#include "../objects/GL_vertexArrayObject.h"
#include "../state/GL_inputAssembler.h"

namespace ts3::gpuapi
{

	GLVertexArrayObjectCache::GLVertexArrayObjectCache() = default;

	GLVertexArrayObjectCache::~GLVertexArrayObjectCache() = default;

	const GLVertexArrayObject & GLVertexArrayObjectCache::getOrCreate(
			const GLIAInputLayoutImmutableStateCompat & pInputLayoutState,
			const GLIAVertexStreamImmutableState & pVertexStreamState )
	{
		GLVertexArrayObjectCachedID cachedID{
			reinterpret_cast<uint64>( &pInputLayoutState ),
			reinterpret_cast<uint64>( &pVertexStreamState )
		};

		auto cachedEntryIter = _persistentVertexArrayObjectMap.find( cachedID );
		if( cachedEntryIter == _persistentVertexArrayObjectMap.end() )
		{
			auto vertexArrayObject = smutil::createGLVertexArrayObjectLayoutOnlyGL( pInputLayoutState.mGLInputLayoutDefinition );

			auto insertResult = _persistentVertexArrayObjectMap.emplace( cachedID, std::move( vertexArrayObject ) );

			cachedEntryIter = insertResult.first;
		}

		return *cachedEntryIter->second;
	}


	const GLVertexArrayObject & GLVertexArrayObjectCache::getOrCreate(
			const GLIAInputLayoutDefinition & pInputLayoutDefinition,
			const GLIAVertexStreamDefinition & pVertexStreamDefinition )
	{
		const auto hash1 = hashCompute<UniqueGPUObjectID::sHashAlgo1>( pInputLayoutDefinition );
		const auto hash2 = hashCompute<UniqueGPUObjectID::sHashAlgo2>( pVertexStreamDefinition );

		UniqueGPUObjectID vaoRefID{ hash1, hash2 };

		auto cachedEntryIter = _transientVertexArrayObjectMap.find( vaoRefID );
		if( cachedEntryIter == _transientVertexArrayObjectMap.end() )
		{
			auto vertexArrayObject = smutil::createGLVertexArrayObjectLayoutStreamCombinedGL(
					pInputLayoutDefinition,
					pVertexStreamDefinition );

			auto insertResult = _transientVertexArrayObjectMap.emplace( vaoRefID, std::move( vertexArrayObject ) );

			cachedEntryIter = insertResult.first;
		}

		return *cachedEntryIter->second;
	}

	void GLVertexArrayObjectCache::reset()
	{
		_persistentVertexArrayObjectMap.clear();
		_transientVertexArrayObjectMap.clear();
	}

}
