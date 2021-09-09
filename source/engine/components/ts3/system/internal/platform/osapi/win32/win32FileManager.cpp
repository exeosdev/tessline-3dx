
#include <ts3/system/fileManagerNative.h>

namespace ts3::system
{

    void _win32OpenFileGeneric( File & pFile, std::string pFilePath, DWORD pFileAccess, DWORD pOpenMode, DWORD pFileFlags );
    void _win32CloseFile( File & pFile );
    DWORD _win32TranslateFileOpenModeToWin32Access( EFileOpenMode pOpenMode );
    DWORD _win32TranslateFileOpenModeToWin32CreationDisposition( EFileOpenMode pOpenMode );
    std::string _win32GenerateTempFileName();

    void FileManager::_nativeOpenFile( File & pFile, std::string pFilePath, EFileOpenMode pOpenMode )
    {
        const auto fileAccess = _win32TranslateFileOpenModeToWin32Access( pOpenMode );
        const auto creationDisposition = _win32TranslateFileOpenModeToWin32CreationDisposition( pOpenMode );

        _win32OpenFileGeneric( pFile, std::move( pFilePath ), fileAccess, creationDisposition, FILE_ATTRIBUTE_NORMAL );
    }

    void FileManager::_nativeCreateFile( File & pFile, std::string pFilePath )
    {
        const auto fileAccess = GENERIC_READ | GENERIC_WRITE;
        const auto creationDisposition = CREATE_ALWAYS;

        _win32OpenFileGeneric( pFile, std::move( pFilePath ), fileAccess, creationDisposition, FILE_ATTRIBUTE_NORMAL );
    }

    void FileManager::_nativeCreateTemporaryFile( File & pFile )
    {
        const auto fileAccess = GENERIC_READ | GENERIC_WRITE;
        const auto creationDisposition = CREATE_ALWAYS;

        auto tempFilePath = _win32GenerateTempFileName();

        _win32OpenFileGeneric( pFile, std::move( tempFilePath ), fileAccess, creationDisposition, FILE_ATTRIBUTE_TEMPORARY );
    }

    void FileManager::_nativeCloseFile( File & pFile )
    {
        _win32CloseFile( pFile );
    }

    void FileManager::_nativeGenerateTemporaryFileName( std::string & pOutName )
    {
        auto tmpFileName = _win32GenerateTempFileName();
        pOutName = std::move( tmpFileName );
    }


    void _win32OpenFileGeneric( File & pFile, std::string pFilePath, DWORD pFileAccess, DWORD pOpenMode, DWORD pFileFlags )
    {
        auto fileHandle = ::CreateFileA( pFilePath.c_str(),
                                         pFileAccess,
                                         0u,
                                         nullptr,
                                         pOpenMode,
                                         pFileFlags,
                                         nullptr );

        if( !fileHandle )
        {
            auto lastErrorCode = ::GetLastError();
            auto errorMessage = platform::wnfQuerySystemErrorMessage( lastErrorCode );

            ts3ThrowAutoEx( E_EXC_SYSTEM_FILE_OPEN_ERROR, std::move( errorMessage ), std::move( pFilePath ) );
        }

        pFile.mPrivate->nativeDataPriv.fileHandle = fileHandle;
        pFile.mPrivate->fullPath = std::move( pFilePath );
    }

    void _win32CloseFile( File & pFile )
    {
        auto closeResult = ::CloseHandle( pFile.mPrivate->nativeDataPriv.fileHandle );
        if( closeResult == FALSE )
        {
            auto lastErrorCode = ::GetLastError();
            auto errorMessage = platform::wnfQuerySystemErrorMessage( lastErrorCode );

            ts3ThrowAutoEx( E_EXC_SYSTEM_FILE_OPEN_ERROR, std::move( errorMessage ), pFile.mPrivate->fullPath );
        }

        pFile.mPrivate->nativeDataPriv.fileHandle = nullptr;
        pFile.mPrivate->fullPath.clear();
        pFile.mPrivate->name.clear();
    }

    DWORD _win32TranslateFileOpenModeToWin32Access( EFileOpenMode pOpenMode )
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

    DWORD _win32TranslateFileOpenModeToWin32CreationDisposition( EFileOpenMode pOpenMode )
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

    std::string _win32GenerateTempFileName()
    {
        file_char_t tempFileDir[MAX_PATH];
        ::GetTempPathA( MAX_PATH, tempFileDir );

        file_char_t tempFileName[MAX_PATH];
        ::GetTempFileNameA( tempFileDir, "TS3", 0, tempFileName );

        return std::string( tempFileName );
    }

}
