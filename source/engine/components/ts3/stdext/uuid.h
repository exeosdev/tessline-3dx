
#include "prerequisites.h"

namespace ts3
{

	inline std::string getUUIDString( const std::string & input )
	{
		std::string result;

		auto lbpos = input.find_first_of( '{', 0 );
		auto rbpos = input.find_first_of( '}', lbpos );

		if( ( lbpos != std::string::npos ) && ( rbpos!= std::string::npos ) )
		{
			auto uuidstr = input.substr( lbpos, rbpos - lbpos + 1 );
			// {xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx}
			if( uuidstr.length() == 38 )
			{
				result = std::move( uuidstr );
			}
		}

		return result;
	}

}
