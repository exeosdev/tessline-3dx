
#include <ts3/system/assetSystemNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

    AssetLoaderHandle createAssetLoader( AssetLoaderCreateInfo pCreateInfo )
    {
        if( !pCreateInfo.sysContext )
        {
            return nullptr;
        }

        auto assetLoader = createSysObject<AssetLoader>( pCreateInfo.sysContext );
        return assetLoader;
    }


    void AssetLoader::_nativeConstructor()
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        mInternal->nativeDataPriv.setSessionData( aSessionData );
        mInternal->nativeDataPriv.aAssetManager = aSessionData.aCommonAppState->activity->assetManager;
    }

    void AssetLoader::_nativeDestructor() noexcept
    {
        mInternal->nativeDataPriv.resetSessionData();
    }

    AAsset * _resolveAssetNameAndOpen( AAssetManager * pAAssetManager, AAssetDir * pAAssetDir, FileUtilityAPI::FilePathInfo & pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags )
    {
        AAsset * aAsset = nullptr;
        bool aAssetDirOwnershipFlag = false;

        if( !pAAssetDir )
        {
            pAAssetDir = AAssetManager_openDir( pAAssetManager, pAssetPathInfo.directory.c_str() );
            aAssetDirOwnershipFlag = true;
        }

        if( pAAssetDir )
        {
            aAsset = AAssetManager_open( pAAssetManager, pAssetPathInfo.fileName.c_str(), AASSET_MODE_RANDOM );

            if( !aAsset && pFlags.isSet( E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT ) )
            {
                while( auto * assetFilename = AAssetDir_getNextFileName( pAAssetDir ) )
                {
                    auto assetNameStr = std::string( assetFilename );
                    auto assetNamePos = assetNameStr.find( pAssetPathInfo.fileName );

                    if( assetNamePos == 0 )
                    {
                        aAsset = AAssetManager_open( pAAssetManager, assetFilename, AASSET_MODE_RANDOM );
                        pAssetPathInfo.fileName = std::move( assetNameStr );
                        break;
                    }
                }

                AAssetDir_rewind( pAAssetDir );
            }

            if( aAssetDirOwnershipFlag )
            {
                AAssetDir_close( pAAssetDir );
                pAAssetDir = nullptr;
                aAssetDirOwnershipFlag = false;
            }
        }

        return aAsset;
    }

    AssetHandle AssetLoader::_nativeOpenSubAsset( FileUtilityAPI::FilePathInfo pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags )
    {
        AssetHandle asset = nullptr;

        auto * aAssetManager = mInternal->nativeDataPriv.aAssetManager;

        if( auto * aAsset = _resolveAssetNameAndOpen( aAssetManager, nullptr, pAssetPathInfo, pFlags ) )
        {
            asset = createSysObject<Asset>( getHandle<AssetLoader>() );
            asset->mInternal->name = std::move( pAssetPathInfo.fileName );
            asset->mInternal->nativeDataPriv.aAsset = aAsset;
            asset->mInternal->nativeDataPriv.aAssetManager = mInternal->nativeDataPriv.aAssetManager;
        }

        return asset;
    }

    AssetDirectoryHandle AssetLoader::_nativeOpenDirectory( std::string pDirectoryName )
    {
        std::string assetDirRefName = "assets";
        assetDirRefName.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        assetDirRefName.append( pDirectoryName );

        AssetDirectoryHandle assetDirectory = nullptr;

        if( auto * aAssetDir = AAssetManager_openDir( mInternal->nativeDataPriv.aAssetManager, pDirectoryName.c_str() ) )
        {
            assetDirectory = createSysObject<AssetDirectory>( getHandle<AssetLoader>() );
            assetDirectory->mInternal->dirName = std::move( pDirectoryName );
            assetDirectory->mInternal->nativeDataPriv.aAssetDir = aAssetDir;
        }

        return assetDirectory;
    }

    bool AssetLoader::_nativeCheckDirectoryExists( const std::string & pDirectoryName ) const
    {;
        std::string assetDirRefName = "assets";
        assetDirRefName.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
        assetDirRefName.append( pDirectoryName );

        auto * aAssetDir = AAssetManager_openDir( mInternal->nativeDataPriv.aAssetManager, assetDirRefName.c_str() );
        if( aAssetDir )
        {
            AAssetDir_close( aAssetDir );
            return true;
        }

        return false;
    }


    void AssetDirectory::_nativeConstructor()
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        mInternal->nativeDataPriv.setSessionData( aSessionData );
        mInternal->nativeDataPriv.aAssetManager = mAssetLoader->mInternal->nativeDataPriv.aAssetManager;
    }

    void AssetDirectory::_nativeDestructor() noexcept
    {
        if( mInternal->nativeDataPriv.aAssetDir )
        {
            AAssetDir_close( mInternal->nativeDataPriv.aAssetDir );
            mInternal->nativeDataPriv.aAssetDir = nullptr;
        }

        mInternal->nativeDataPriv.aAssetManager = nullptr;
        mInternal->nativeDataPriv.resetSessionData();
    }

    void AssetDirectory::_nativeRefreshAssetList()
    {
        ts3DebugAssert( mInternal->assetNameList.empty() );

        while( auto * aAssetName = AAssetDir_getNextFileName( mInternal->nativeDataPriv.aAssetDir ) )
        {
            mInternal->assetNameList.push_back( std::string( aAssetName ) );
        }

        AAssetDir_rewind( mInternal->nativeDataPriv.aAssetDir );
    }

    AssetHandle AssetDirectory::_nativeOpenAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags )
    {
        AssetHandle asset = nullptr;

        FileUtilityAPI::FilePathInfo assetPathInfo;
        assetPathInfo.directory = mInternal->dirName;
        assetPathInfo.fileName = std::move( pAssetName );

        auto * aAssetManager = mInternal->nativeDataPriv.aAssetManager;
        auto * aAssetDir = mInternal->nativeDataPriv.aAssetDir;

        if( auto * aAsset = _resolveAssetNameAndOpen( aAssetManager, aAssetDir, assetPathInfo, pFlags ) )
        {
            asset = createSysObject<Asset>( getHandle<AssetLoader>() );
            asset->mInternal->name = std::move( assetPathInfo.fileName );
            asset->mInternal->nativeDataPriv.aAsset = aAsset;
            asset->mInternal->nativeDataPriv.aAssetManager = mInternal->nativeDataPriv.aAssetManager;
        }

        return asset;
    }

    bool AssetDirectory::_nativeCheckAssetExists( const std::string & pAssetName ) const
    {
        if( auto * aAsset = AAssetManager_open( mInternal->nativeDataPriv.aAssetManager, pAssetName.c_str(), AASSET_MODE_BUFFER ) )
        {
            AAsset_close( aAsset );
            return true;
        }
        return false;
    }


    void Asset::_nativeConstructor()
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        mInternal->nativeDataPriv.setSessionData( aSessionData );
    }

    void Asset::_nativeDestructor() noexcept
    {
        if( mInternal->nativeDataPriv.aAsset )
        {
            AAsset_close( mInternal->nativeDataPriv.aAsset );
            mInternal->nativeDataPriv.aAsset = nullptr;
        }
    }

    file_offset_t Asset::_nativeSetReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return 0;
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
