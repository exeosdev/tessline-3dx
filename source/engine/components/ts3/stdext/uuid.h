
#include "prerequisites.h"

namespace ts3
{

	inline std::string getUUIDString( const std::string & pInput, char pLeftDelim = '{' )
	{
		std::string result;

		char rightDelim = 0;

		switch( pLeftDelim )
		{
		    case '{': rightDelim = '}'; break;
		    case '[': rightDelim = ']'; break;
		    case '<': rightDelim = '>'; break;
		    case '$': rightDelim = '$'; break;
		}

		if( rightDelim != 0 )
		{
		    auto lbPos = pInput.find_first_of( pLeftDelim );
		    auto rbPos = pInput.find_first_of( rightDelim, lbPos );

		    if( ( lbPos != std::string::npos ) && ( rbPos != std::string::npos ) )
		    {
		        auto uuidstr = pInput.substr( lbPos, rbPos - lbPos + 1 );
                // #xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx#
		        if( uuidstr.length() == 38 )
		        {
		            result = std::move( uuidstr );
		        }
		    }
		}

		return result;
	}

}
