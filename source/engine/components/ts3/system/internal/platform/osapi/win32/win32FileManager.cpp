
#include <ts3/system/fileManagerNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    HANDLE _win32OpenFileGeneric( const char * pFilePath, DWORD pFileAccess, DWORD pOpenMode, DWORD pFileFlags );
    void _win32CloseFile( HANDLE pFileHandle, const char * pFilePath );
    std::string _win32GenerateTempFileName();


    void File::_nativeConstructor()
    {}

    void File::_nativeDestructor() noexcept
    {
        if( mInternal->nativeDataPriv.fileHandle )
        {
            _win32CloseFile( mInternal->nativeDataPriv.fileHandle, mInternal->fullPath.c_str() );
            mInternal->nativeDataPriv.fileHandle = nullptr;
        }
    }

    file_size_t File::_nativeReadData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize )
    {
        DWORD readBytesNum = 0u;
        auto readResult = ::ReadFile( mInternal->nativeDataPriv.fileHandle, pBuffer, pBufferSize, &readBytesNum, nullptr );

        if( !readResult )
        {
            auto errorCode = ::GetLastError();
            auto errorMessage = platform::mseQuerySystemErrorMessage( errorCode );
            ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
        }

        return trunc_numeric_cast<file_size_t>( readBytesNum );
    }

    file_offset_t File::_nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        LARGE_INTEGER u64FileOffset;
        u64FileOffset.QuadPart = static_cast<decltype( u64FileOffset.QuadPart )>( pOffset );

        auto win32FPMoveMode = platform::win32TranslateFilePointerRefPos( pRefPos );

        u64FileOffset.LowPart = ::SetFilePointer( mInternal->nativeDataPriv.fileHandle,
                                                  u64FileOffset.LowPart,
                                                  &( u64FileOffset.HighPart ),
                                                  win32FPMoveMode );

        if( u64FileOffset.LowPart == INVALID_SET_FILE_POINTER )
        {
            auto lastError = ::GetLastError();
            if( lastError != NO_ERROR )
            {
                auto errorMessage = platform::mseQuerySystemErrorMessage( lastError );
                ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
            }
        }

        return trunc_numeric_cast<file_offset_t>( u64FileOffset.QuadPart );
    }

    file_size_t File::_nativeGetSize() const
    {
        ULARGE_INTEGER u64FileSize;
        u64FileSize.LowPart = ::GetFileSize( mInternal->nativeDataPriv.fileHandle, &( u64FileSize.HighPart ) );

        return trunc_numeric_cast<file_size_t>( u64FileSize.QuadPart );
    }


    void FileManager::_nativeConstructor()
    {}

    void FileManager::_nativeDestructor() noexcept
    {}

    void FileManager::_nativeOpenFile( File & pFile, std::string pFilePath, EFileOpenMode pOpenMode )
    {
        const auto fileAccess = platform::win32TranslateFileOpenModeToWin32Access( pOpenMode );
        const auto creationDisposition = platform::win32TranslateFileOpenModeToWin32CreationDisposition( pOpenMode );

        auto fileHandle = _win32OpenFileGeneric( pFilePath.c_str(), fileAccess, creationDisposition, FILE_ATTRIBUTE_NORMAL );
        pFile.mInternal->fullPath = std::move( pFilePath );
        pFile.mInternal->nativeDataPriv.fileHandle = fileHandle;
    }

    void FileManager::_nativeCreateFile( File & pFile, std::string pFilePath )
    {
        const auto fileAccess = GENERIC_READ | GENERIC_WRITE;
        const auto creationDisposition = CREATE_ALWAYS;

        auto fileHandle = _win32OpenFileGeneric( pFilePath.c_str(), fileAccess, creationDisposition, FILE_ATTRIBUTE_NORMAL );
        pFile.mInternal->fullPath = std::move( pFilePath );
        pFile.mInternal->nativeDataPriv.fileHandle = fileHandle;
    }

    void FileManager::_nativeCreateTemporaryFile( File & pFile )
    {
        const auto fileAccess = GENERIC_READ | GENERIC_WRITE;
        const auto creationDisposition = CREATE_ALWAYS;

        auto tempFilePath = _win32GenerateTempFileName();

        auto fileHandle = _win32OpenFileGeneric( tempFilePath.c_str(), fileAccess, creationDisposition, FILE_ATTRIBUTE_TEMPORARY );
        pFile.mInternal->fullPath = std::move( tempFilePath );
        pFile.mInternal->nativeDataPriv.fileHandle = fileHandle;
    }

    FileNameList FileManager::_nativeEnumDirectoryFileNameList( const std::string & pDirectory )
    {
        FileNameList resultList;

        auto targetDirectory = pDirectory;
        targetDirectory.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        targetDirectory.append( 1, '*' );

        WIN32_FIND_DATAA win32FindFileData;
        auto win32FindFileHandle = ::FindFirstFileA( targetDirectory.c_str(), &win32FindFileData );

        if( win32FindFileHandle == INVALID_HANDLE_VALUE )
        {
            auto lastError = ::GetLastError();
            auto lastErrorMessage = platform::mseQuerySystemErrorMessage( lastError );
            ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( lastErrorMessage ) );
        }

        while( win32FindFileHandle )
        {
            auto fileName = std::string( win32FindFileData.cFileName );
            auto fileAttributes = makeBitmask( win32FindFileData.dwFileAttributes );

            if( !fileAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY ) && ( fileName != "." ) && ( fileName != ".." ) )
            {
                resultList.push_back( std::string( win32FindFileData.cFileName ) );
            }

            auto findNextResult = ::FindNextFileA( win32FindFileHandle, &win32FindFileData );

            if( !findNextResult )
            {
                auto lastError = ::GetLastError();
                if( lastError == ERROR_NO_MORE_FILES )
                {
                    ::FindClose( win32FindFileHandle );
                    win32FindFileHandle = nullptr;
                }
                else
                {
                    auto lastErrorMessage = platform::mseQuerySystemErrorMessage( lastError );
                    ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( lastErrorMessage ) );
                }
            }
        }

        return resultList;
    }

    std::string FileManager::_nativeGenerateTemporaryFileName()
    {
        auto tmpFileName = _win32GenerateTempFileName();
        return std::string( tmpFileName );
    }

    bool FileManager::_nativeCheckDirectoryExists( const std::string & pDirPath )
    {
        Bitmask<DWORD> targetAttributes = ::GetFileAttributesA( pDirPath.c_str() );
        return ( targetAttributes != INVALID_FILE_ATTRIBUTES ) && targetAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY );
    }

    bool FileManager::_nativeCheckFileExists( const std::string & pFilePath )
    {
        Bitmask<DWORD> targetAttributes = ::GetFileAttributesA( pFilePath.c_str() );
        return ( targetAttributes != INVALID_FILE_ATTRIBUTES ) && !targetAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY );
    }


    HANDLE _win32OpenFileGeneric( const char * pFilePath, DWORD pFileAccess, DWORD pOpenMode, DWORD pFileFlags )
    {
        auto fileHandle = ::CreateFileA( pFilePath,
                                         pFileAccess,
                                         0u,
                                         nullptr,
                                         pOpenMode,
                                         pFileFlags,
                                         nullptr );

        if( !fileHandle )
        {
            auto lastErrorCode = ::GetLastError();
            auto errorMessage = platform::mseQuerySystemErrorMessage( lastErrorCode );

            ts3ThrowDesc( E_EXC_SYSTEM_FILE_OPEN_ERROR, std::move( errorMessage ), std::move( pFilePath ) );
        }

        return fileHandle;
    }

    void _win32CloseFile( HANDLE pFileHandle, const char * pFilePath )
    {
        auto closeResult = ::CloseHandle( pFileHandle );

        if( closeResult == FALSE )
        {
            auto lastErrorCode = ::GetLastError();
            auto errorMessage = platform::mseQuerySystemErrorMessage( lastErrorCode );
            ts3DebugInterrupt();
        }
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
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
