
#include "win32FileAPI.h"

namespace ts3::system
{

    namespace platform
    {

        DWORD win32TranslateFileOpenModeToWin32Access( EFileOpenMode pOpenMode )
        {
            switch( pOpenMode )
            {
            case EFileOpenMode::ReadOnly:
                return GENERIC_READ;

            case EFileOpenMode::ReadWrite:
                return GENERIC_READ | GENERIC_WRITE;

            case EFileOpenMode::WriteAppend:
                return GENERIC_WRITE;

            case EFileOpenMode::WriteOverwrite:
                return GENERIC_READ | GENERIC_WRITE;

            default:
                break;
            }
            return GENERIC_READ | GENERIC_WRITE;
        }

        DWORD win32TranslateFileOpenModeToWin32CreationDisposition( EFileOpenMode pOpenMode )
        {
            switch( pOpenMode )
            {
            case EFileOpenMode::ReadOnly:
                return OPEN_EXISTING;

            case EFileOpenMode::ReadWrite:
                return OPEN_ALWAYS;

            case EFileOpenMode::WriteAppend:
                return OPEN_ALWAYS;

            case EFileOpenMode::WriteOverwrite:
                return CREATE_ALWAYS;

            default:
                break;
            }
            return OPEN_ALWAYS;
        }

        DWORD win32TranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos )
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
