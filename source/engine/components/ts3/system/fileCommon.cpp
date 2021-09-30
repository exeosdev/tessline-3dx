
#include "fileCommon.h"
#include <ts3/stdext/stringUtils.h>

namespace ts3::system
{

    std::string FileAPI::normalizePath( const std::string & pPath )
    {
        auto localPathString = pPath;
        std::replace( localPathString.begin(),
                      localPathString.end(),
                      TS3_PCL_ENV_NON_STANDARD_PATH_DELIMITER,
                      TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );

        using StringVec = std::vector<std::string>;
        auto pathComponents = strUtils::splitStringEx<StringVec>(
                localPathString, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER,
                []( StringVec & pVec, const char * pStr, size_t pLength ) -> void {
                    if( pLength > 0 )
                    {
                        pVec.push_back( std::string( pStr, pLength ) );
                    }
                });

        auto resultLength = localPathString.length();

        for( size_t pathComponentIndex = 0; pathComponentIndex < pathComponents.size();  )
        {
            auto & component = pathComponents[pathComponentIndex];

            if( component == "." )
            {
                pathComponents.erase( pathComponents.begin() + pathComponentIndex );
                resultLength -= 2;
            }
            else if( component == ".." )
            {
                if( pathComponentIndex == 0 )
                {
                    ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
                }

                auto & prevComponent = pathComponents[pathComponentIndex - 1];
                auto removalSize = 3 + prevComponent.length() + 1;

                pathComponents.erase( pathComponents.begin() + pathComponentIndex );
                pathComponents.erase( pathComponents.begin() + pathComponentIndex - 1 );

                resultLength -= removalSize;
                pathComponentIndex -= 1;
            }
            else
            {
                pathComponentIndex += 1;
            }
        }

        std::string result;
        result.reserve( resultLength );

        for( size_t pathComponentIndex = 0; pathComponentIndex < pathComponents.size(); ++pathComponentIndex )
        {
            if( pathComponentIndex > 0 )
            {
                result.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
            }
            result.append( pathComponents[pathComponentIndex] );
        }

        return result;
    }

}
