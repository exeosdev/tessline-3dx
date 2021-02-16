
#include <ts3/engine/rcdata/commonRDUtils.h>
#include <fstream>

namespace ts3
{

	ResourceFileDataBuffer loadResourceFileContent( const char * pFilename )
	{
		if( !pFilename )
		{
			return nullptr;
		}

		std::ifstream inputFile( pFilename, std::ios::in );
		if( !inputFile )
		{
			return nullptr;
		}

		inputFile.seekg( 0, std::ios::end );

		size_t fileSize = inputFile.tellg();
		if( fileSize == static_cast<std::ifstream::pos_type>( -1 ) )
		{
			return nullptr;
		}

		inputFile.seekg( 0, std::ios::beg );

		ResourceFileDataBuffer fileDataBuffer;
		fileDataBuffer.resize( fileSize );
		inputFile.read( fileDataBuffer.dataPtrAs<char>(), fileSize );

		return fileDataBuffer;
	}

	ResourceFileDataBuffer loadResourceFileContent( const std::string & pFilename )
	{
		if( pFilename.empty() )
		{
			return nullptr;
		}

		return loadResourceFileContent( pFilename.c_str() );
	}

}
