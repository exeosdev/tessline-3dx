
#pragma once

#ifndef __TS3_ENGINE_COMMON_RD_UTILS_H__
#define __TS3_ENGINE_COMMON_RD_UTILS_H__

#include "../prerequisites.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	using ResourceFileDataBuffer = ts3::DynamicMemoryBuffer;

	TS3_ENGINE_API ResourceFileDataBuffer loadResourceFileContent( const char * pFilename );

	TS3_ENGINE_API ResourceFileDataBuffer loadResourceFileContent( const std::string & pFilename );

	template <typename TpResult>
	inline TpResult loadResourceFromFile( const char * pFilename, TpResult( * pCallback )( const void *, size_t ) )
	{
		const auto fileDataBuffer = loadResourceFileContent( pFilename );
		return pCallback( fileDataBuffer.dataPtr(), fileDataBuffer.size() );
	}

	template <typename TpResult>
	inline TpResult loadResourceFromFile( const char * pFilename, TpResult( * pCallback )( const ResourceFileDataBuffer & ) )
	{
		const auto fileDataBuffer = loadResourceFileContent( pFilename );
		return pCallback( fileDataBuffer );
	}

	template <typename TpResult>
	inline TpResult loadResourceFromFile( const std::string & pFilename, TpResult( * pCallback )( const void *, size_t ) )
	{
		const auto fileDataBuffer = loadResourceFileContent( pFilename );
		return pCallback( fileDataBuffer.dataPtr(), fileDataBuffer.size() );
	}

	template <typename TpResult>
	inline TpResult loadResourceFromFile( const std::string & pFilename, TpResult( * pCallback )( const ResourceFileDataBuffer & ) )
	{
		const auto fileDataBuffer = loadResourceFileContent( pFilename );
		return pCallback( fileDataBuffer );
	}

}

#endif // __TS3_ENGINE_COMMON_RD_UTILS_H__
