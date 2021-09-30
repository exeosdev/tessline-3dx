
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

    FileAPI::FilePathInfo FileAPI::splitFilePath( std::string pFilePath, Bitmask<EFileAPIFlags> pFlags )
    {
        FilePathInfo pathInfo;

        if( !pFilePath.empty() )
        {
            if( !pFlags.isSetAnyOf( E_FILE_API_FLAG_SPLIT_PATH_ASSUME_DIRECTORY_BIT | E_FILE_API_FLAG_SPLIT_PATH_ASSUME_FILE_BIT ) )
            {
                pFlags.set( E_FILE_API_FLAG_SPLIT_PATH_ASSUME_DIRECTORY_BIT );
            }
            else if( pFlags.isSet( E_FILE_API_FLAG_SPLIT_PATH_ASSUME_DIRECTORY_BIT | E_FILE_API_FLAG_SPLIT_PATH_ASSUME_FILE_BIT ) )
            {
                pFlags.unset( E_FILE_API_FLAG_SPLIT_PATH_ASSUME_FILE_BIT );
            }

            auto lastSeparatorPos = pFilePath.find_last_of( TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
            if( lastSeparatorPos == std::string::npos )
            {
                if( isFilenameWithExtension( pFilePath ) || pFlags.isSet( E_FILE_API_FLAG_SPLIT_PATH_ASSUME_FILE_BIT ) )
                {
                    pathInfo.fileName = pFilePath;
                }
            }
            else
            {
                auto directoryPart = pFilePath.substr( 0, lastSeparatorPos );
                auto filenamePath = pFilePath.substr( lastSeparatorPos + 1 );

                if( isFilenameWithExtension( filenamePath ) || pFlags.isSet( E_FILE_API_FLAG_SPLIT_PATH_ASSUME_FILE_BIT ) )
                {
                    pathInfo.directory = std::move( directoryPart );
                    pathInfo.fileName = std::move( filenamePath );
                }
            }

            if( pathInfo.directory.empty() && pathInfo.fileName.empty() )
            {
                pathInfo.directory = std::move( pFilePath );
            }
        }

        return pathInfo;
    }

    bool FileAPI::isFilenameWithExtension( const std::string & pFilename )
    {
        if( !pFilename.empty() )
        {
            auto dotPos = pFilename.find_last_of( '.' );
            if( ( dotPos != std::string::npos ) && ( dotPos != 0 ) && ( dotPos != ( pFilename.length() - 1 ) ) )
            {
                return true;
            }
        }
        return false;
    }

}
