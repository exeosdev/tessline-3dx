
#include <ts3/core/prerequisites.h>

namespace ts3
{

	std::string FileLocationInfo::toString() const
	{
		auto funcStrLength = std::strlen( function );
		auto fileStrLength = std::strlen( filename );

		std::string result{};
		result.reserve( funcStrLength + fileStrLength + 128 );

		result.append( "Function " );
		result.append( function, funcStrLength );
		result.append( " in file " );
		result.append( filename, fileStrLength );

	#if defined( TS3_PCL_PROJECT_ROOT_PATH )
		auto replacePos = result.find( TS3_PCL_PROJECT_ROOT_PATH );
		auto replaceLength = std::strlen( TS3_PCL_PROJECT_ROOT_PATH );
		if( ( replacePos != std::string::npos ) && ( replaceLength > 0 ) )
		{
			result.erase( replacePos, replaceLength );
		}
	#endif

		result.append( ", line " );
		result.append( std::to_string( lineNumber ) );

		return result;

	}

}
