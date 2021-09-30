
#include <ts3/system/assetSystemNative.h>
#include <ts3/system/sysContextNative.h>
#include "win32FileAPI.h"

namespace ts3::system
{

    AssetLoaderHandle createAssetLoader( AssetLoaderCreateInfo pCreateInfo )
    {
        if( !pCreateInfo.sysContext || !platform::win32FACheckDirectoryExists( pCreateInfo.nativeParams.relativeAssetRootDir ) )
        {
            return nullptr;
        }

        auto assetLoader = createSysObject<AssetLoader>( pCreateInfo.sysContext );

        auto assetDirectory = SysContext::queryCurrentProcessExecutableDirectory();
        assetDirectory.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        assetDirectory.append( pCreateInfo.nativeParams.relativeAssetRootDir );
        assetDirectory = FileUtilityAPI::normalizePath( assetDirectory );

        assetLoader->mInternal->nativeDataPriv.rootDir = std::move( assetDirectory );

        return assetLoader;
    }


    void AssetLoader::_nativeConstructor()
    {}

    void AssetLoader::_nativeDestructor() noexcept
    {}

    AssetHandle AssetLoader::_nativeOpenSubAsset( FileUtilityAPI::FilePathInfo pAssetPathInfo )
    {
        AssetHandle asset = nullptr;

        auto combinedFilePath = mInternal->nativeDataPriv.rootDir;
        combinedFilePath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        combinedFilePath.append( pAssetPathInfo.directory );

        if( platform::win32FACheckDirectoryExists( combinedFilePath ) )
        {
            combinedFilePath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
            combinedFilePath.append( pAssetPathInfo.fileName );

            if( platform::win32FACheckFileExists( combinedFilePath ) )
            {
                auto fileHandle = ::CreateFileA( combinedFilePath.c_str(),
                                                 GENERIC_READ,
                                                 0u,
                                                 nullptr,
                                                 OPEN_EXISTING,
                                                 FILE_ATTRIBUTE_NORMAL,
                                                 nullptr );

                if( !fileHandle )
                {
                    auto errorMessage = platform::mseQuerySystemErrorMessage( ::GetLastError() );
                    ts3ThrowAutoEx( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
                }

                asset = createSysObject<Asset>( getHandle<AssetLoader>() );
                asset->mInternal->name = std::move( pAssetPathInfo.fileName );
                asset->mInternal->nativeDataPriv.fileHandle = fileHandle;
                asset->mInternal->nativeDataPriv.filePath = std::move( combinedFilePath );
            }
        }

        return asset;
    }

    AssetDirectoryHandle AssetLoader::_nativeOpenDirectory( std::string pDirectoryName )
    {
        auto combinedDirPath = mInternal->nativeDataPriv.rootDir;
        combinedDirPath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        combinedDirPath.append( pDirectoryName );

        if( !platform::win32FACheckDirectoryExists( combinedDirPath ) )
        {
            return nullptr;
        }

        auto assetDirectory = createSysObject<AssetDirectory>( getHandle<AssetLoader>() );
        assetDirectory->mInternal->dirName = std::move( pDirectoryName );
        assetDirectory->mInternal->nativeDataPriv.combinedDirPath = std::move( combinedDirPath );

        return assetDirectory;
    }

    bool AssetLoader::_nativeCheckDirectoryExists( const std::string & pDirectoryName ) const
    {
        auto combinedDirPath = mInternal->nativeDataPriv.rootDir;
        combinedDirPath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        combinedDirPath.append( pDirectoryName );

        return platform::win32FACheckDirectoryExists( combinedDirPath );
    }


    void AssetDirectory::_nativeConstructor()
    {}

    void AssetDirectory::_nativeDestructor() noexcept
    {}

    void AssetDirectory::_nativeRefreshAssetList()
    {
        ts3DebugAssert( mInternal->assetNameList.empty() );

        auto targetDirectory = mInternal->nativeDataPriv.combinedDirPath;
        targetDirectory.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        targetDirectory.append( 1, '*' );

        WIN32_FIND_DATAA win32FindFileData;
        auto win32FindFileHandle = ::FindFirstFileA( targetDirectory.c_str(), &win32FindFileData );

        if( win32FindFileHandle == INVALID_HANDLE_VALUE )
        {
            auto lastError = ::GetLastError();
            auto lastErrorMessage = platform::mseQuerySystemErrorMessage( lastError );
            ts3ThrowAutoEx( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( lastErrorMessage ) );
        }

        while( win32FindFileHandle )
        {
            std::string assetFileName = win32FindFileData.cFileName;
            if( ( assetFileName != "." ) && ( assetFileName != ".." ) )
            {
                mInternal->assetNameList.push_back( std::move( assetFileName ) );
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
                    ts3ThrowAutoEx( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( lastErrorMessage ) );
                }
            }
        }
    }

    AssetHandle AssetDirectory::_nativeOpenAsset( std::string pAssetName )
    {
        auto combinedFilePath = mInternal->nativeDataPriv.combinedDirPath;
        combinedFilePath.append( 1, '\\' );
        combinedFilePath.append( pAssetName );

        if( !platform::win32FACheckFileExists( combinedFilePath ) )
        {
            return nullptr;
        }

        auto assetObject = createSysObject<Asset>( getHandle<AssetDirectory>() );

        auto fileHandle = ::CreateFileA( combinedFilePath.c_str(),
                                         GENERIC_READ,
                                         0u,
                                         nullptr,
                                         OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         nullptr );

        if( !fileHandle )
        {
            auto errorMessage = platform::mseQuerySystemErrorMessage( ::GetLastError() );
            ts3ThrowAutoEx( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
        }

        assetObject->mInternal->nativeDataPriv.fileHandle = fileHandle;
        assetObject->mInternal->nativeDataPriv.filePath = std::move( combinedFilePath );
    }

    bool AssetDirectory::_nativeCheckAssetExists( const std::string & pAssetName ) const
    {
        auto combinedFilePath = mInternal->nativeDataPriv.combinedDirPath;
        combinedFilePath.append( pAssetName );

        return platform::win32FACheckFileExists( combinedFilePath );
    }


    void Asset::_nativeConstructor()
    {}

    void Asset::_nativeDestructor() noexcept
    {
        if( mInternal->nativeDataPriv.fileHandle )
        {
            ::CloseHandle( mInternal->nativeDataPriv.fileHandle );
            mInternal->nativeDataPriv.fileHandle = nullptr;
        }
    }

    file_offset_t Asset::_nativeSetReadPointer( EFilePointerRefPos pRefPos, file_offset_t pOffset )
    {
        LARGE_INTEGER u64FileOffset;
        u64FileOffset.QuadPart = static_cast<decltype( u64FileOffset.QuadPart )>( pOffset );

        auto win32FPMoveMode = platform::win32FATranslateFilePointerRefPos( pRefPos );

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
                ts3ThrowAutoEx( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
            }
        }

        return trunc_numeric_cast<file_offset_t>( u64FileOffset.QuadPart );
    }

}
