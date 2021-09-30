
#include "win32FileAPI.h"

namespace ts3::system
{

    namespace platform
    {

        bool win32FACheckDirectoryExists( const std::string & pDirPath )
        {
            const DWORD directoryFlags = FILE_ATTRIBUTE_DIRECTORY;
            Bitmask<DWORD> targetFileAttributes = ::GetFileAttributesA( pDirPath.c_str() );
            return ( targetFileAttributes != INVALID_FILE_ATTRIBUTES ) && targetFileAttributes.isSetAnyOf( directoryFlags );
        }

        bool win32FACheckFileExists( const std::string & pDirPath )
        {
            const DWORD fileFlags = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_COMPRESSED;
            Bitmask<DWORD> targetFileAttributes = ::GetFileAttributesA( pDirPath.c_str() );
            return ( targetFileAttributes != INVALID_FILE_ATTRIBUTES ) && targetFileAttributes.isSetAnyOf( fileFlags );
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
