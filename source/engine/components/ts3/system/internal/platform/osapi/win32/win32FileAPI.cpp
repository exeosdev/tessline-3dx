
#include "win32FileAPI.h"

namespace ts3::system
{

    namespace platform
    {

        bool win32FACheckDirectoryExists( const std::string & pDirPath )
        {
            Bitmask<DWORD> targetFileAttributes = ::GetFileAttributesA( pDirPath.c_str() );
            return ( targetFileAttributes != INVALID_FILE_ATTRIBUTES ) && targetFileAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY );
        }

        bool win32FACheckFileExists( const std::string & pFilePath )
        {
            Bitmask<DWORD> targetFileAttributes = ::GetFileAttributesA( pFilePath.c_str() );
            return ( targetFileAttributes != INVALID_FILE_ATTRIBUTES ) && !targetFileAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY );
        }

        DWORD win32FATranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos )
        {
            DWORD win32FPMoveMode = 0;

            switch( pFileRefPos )
            {
                case EFilePointerRefPos::FileBeginning:
                {
                    win32FPMoveMode = FILE_BEGIN;
                    break;
                }
                case EFilePointerRefPos::FileEnd:
                {
                    win32FPMoveMode = FILE_END;
                    break;
                }
                case EFilePointerRefPos::PtrCurrent:
                {
                    win32FPMoveMode = FILE_CURRENT;
                    break;
                }
            }

            return win32FPMoveMode;
        }

    }

} // namespace ts3::system
