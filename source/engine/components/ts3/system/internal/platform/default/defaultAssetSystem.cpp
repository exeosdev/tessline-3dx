
#include <ts3/system/assetSystemNative.h>
#include <ts3/system/sysContextNative.h>
#include <ts3/system/fileManagerNative.h>

namespace ts3::system
{

    std::string _resolveFileName( FileUtilityAPI::FilePathInfo & pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags );


    AssetLoaderHandle createAssetLoader( AssetLoaderCreateInfo pCreateInfo )
    {
        if( !pCreateInfo.sysContext || !FileManager::checkDirectoryExists( pCreateInfo.nativeParams.relativeAssetRootDir ) )
        {
            return nullptr;
        }

        auto assetLoader = createSysObject<AssetLoader>( pCreateInfo.sysContext );

        auto assetDirectory = SysContext::queryCurrentProcessExecutableDirectory();
        assetDirectory.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        assetDirectory.append( pCreateInfo.nativeParams.relativeAssetRootDir );
        assetDirectory = FileUtilityAPI::normalizePath( assetDirectory );

        assetLoader->mInternal->nativeDataPriv.fileManager = pCreateInfo.nativeParams.fileManager;
        assetLoader->mInternal->nativeDataPriv.rootDir = std::move( assetDirectory );

        if( !assetLoader->mInternal->nativeDataPriv.fileManager )
        {
            assetLoader->mInternal->nativeDataPriv.fileManager = createSysObject<FileManager>( pCreateInfo.sysContext );
        }

        return assetLoader;
    }


    void AssetLoader::_nativeConstructor()
    {}

    void AssetLoader::_nativeDestructor() noexcept
    {}

    AssetHandle AssetLoader::_nativeOpenSubAsset( FileUtilityAPI::FilePathInfo pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags )
    {
        AssetHandle asset = nullptr;

        auto fileDirectoryPath = mInternal->nativeDataPriv.rootDir;
        fileDirectoryPath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        fileDirectoryPath.append( pAssetPathInfo.directory );

        pAssetPathInfo.directory = std::move( fileDirectoryPath );

        if( FileManager::checkDirectoryExists( pAssetPathInfo.directory ) )
        {
            auto combinedFilePath = _resolveFileName( pAssetPathInfo, pFlags );

            if( FileManager::checkFileExists( combinedFilePath ) )
            {
                auto fileHandle = mInternal->nativeDataPriv.fileManager->openFile( combinedFilePath, EFileOpenMode::ReadOnly );

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

        if( !FileManager::checkDirectoryExists( combinedDirPath ) )
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

        return FileManager::checkDirectoryExists( combinedDirPath );
    }


    void AssetDirectory::_nativeConstructor()
    {}

    void AssetDirectory::_nativeDestructor() noexcept
    {}

    void AssetDirectory::_nativeRefreshAssetList()
    {
        ts3DebugAssert( mInternal->assetNameList.empty() );
        mInternal->assetNameList = FileManager::enumDirectoryFiles( mInternal->nativeDataPriv.combinedDirPath );
    }

    AssetHandle AssetDirectory::_nativeOpenAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags )
    {
        AssetHandle asset = nullptr;

        FileUtilityAPI::FilePathInfo assetPathInfo;
        assetPathInfo.directory = mInternal->nativeDataPriv.combinedDirPath;
        assetPathInfo.fileName = std::move( pAssetName );

        if( FileManager::checkDirectoryExists( mInternal->nativeDataPriv.combinedDirPath ) )
        {
            auto combinedFilePath = _resolveFileName( assetPathInfo, pFlags );

            if( FileManager::checkFileExists( combinedFilePath ) )
            {
                auto fileManager = mAssetLoader->mInternal->nativeDataPriv.fileManager;
                auto fileHandle = fileManager->openFile( combinedFilePath, EFileOpenMode::ReadOnly );

                asset = createSysObject<Asset>( getHandle<AssetDirectory>() );
                asset->mInternal->name = std::move( assetPathInfo.fileName );
                asset->mInternal->nativeDataPriv.fileHandle = fileHandle;
                asset->mInternal->nativeDataPriv.filePath = std::move( combinedFilePath );
            }
        }

        return asset;
    }

    bool AssetDirectory::_nativeCheckAssetExists( const std::string & pAssetName ) const
    {
        auto combinedFilePath = mInternal->nativeDataPriv.combinedDirPath;
        combinedFilePath.append( pAssetName );

        return FileManager::checkFileExists( combinedFilePath );
    }


    void Asset::_nativeConstructor()
    {}

    void Asset::_nativeDestructor() noexcept
    {
        mInternal->nativeDataPriv.fileHandle.reset();
    }

    file_offset_t Asset::_nativeSetReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return mInternal->nativeDataPriv.fileHandle->setFilePointer( pOffset, pRefPos );
    }


    std::string _resolveFileName( FileUtilityAPI::FilePathInfo & pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags )
    {
        auto combinedFilePath = pAssetPathInfo.directory;
        combinedFilePath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        combinedFilePath.append( pAssetPathInfo.fileName );

        if( !FileManager::checkFileExists( combinedFilePath ) )
        {
            combinedFilePath.clear();

            if( pFlags.isSet( E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT ) )
            {
                auto fileNameList = FileManager::enumDirectoryFiles( pAssetPathInfo.directory );
                if( !fileNameList.empty() )
                {
                    for( auto & fileName : fileNameList )
                    {
                        auto assetNamePos = fileName.find( pAssetPathInfo.fileName );
                        if( assetNamePos == 0 )
                        {
                            pAssetPathInfo.fileName = std::move( fileName );
                            combinedFilePath = pAssetPathInfo.directory;
                            combinedFilePath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
                            combinedFilePath.append( pAssetPathInfo.fileName );

                            break;
                        }
                    }
                }
            }
        }

        return combinedFilePath;
    }

}
