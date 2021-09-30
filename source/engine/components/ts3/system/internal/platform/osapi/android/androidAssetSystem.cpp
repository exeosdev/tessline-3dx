
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

    AssetHandle AssetLoader::_nativeOpenSubAsset( FileUtilityAPI::FilePathInfo pAssetPathInfo )
    {
        AssetHandle asset = nullptr;

        if( auto * aAssetDir = AAssetManager_openDir( mInternal->nativeDataPriv.aAssetManager, pAssetPathInfo.directory.c_str() ) )
        {
            if( auto * aAsset = AAssetManager_open( mInternal->nativeDataPriv.aAssetManager, pAssetPathInfo.fileName.c_str(), AASSET_MODE_RANDOM ) )
            {
                asset = createSysObject<Asset>( nullptr );
                asset->mInternal->name = std::move( pAssetPathInfo.fileName );
                asset->mInternal->nativeDataPriv.aAsset = aAsset;
                asset->mInternal->nativeDataPriv.aAssetManager = mInternal->nativeDataPriv.aAssetManager;
            }
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
    }

    void AssetDirectory::_nativeDestructor() noexcept
    {
        if( mInternal->nativeDataPriv.aAssetDir )
        {
            AAssetDir_close( mInternal->nativeDataPriv.aAssetDir );
            mInternal->nativeDataPriv.aAssetDir = nullptr;
        }
        mInternal->nativeDataPriv.resetSessionData();
    }

    void AssetDirectory::_nativeRefreshAssetList()
    {
        ts3DebugAssert( mInternal->assetNameList.empty() );

        while( auto * aAssetName = AAssetDir_getNextFileName( mInternal->nativeDataPriv.aAssetDir ) )
        {
            mInternal->assetNameList.push_back( std::string( aAssetName ) );
        }
    }

    AssetHandle AssetDirectory::_nativeOpenAsset( std::string pAssetName )
    {
        AssetHandle asset = nullptr;

        if( auto * aAsset = AAssetManager_open( mInternal->nativeDataPriv.aAssetManager, pAssetName.c_str(), AASSET_MODE_RANDOM ) )
        {
            asset = createSysObject<Asset>( getHandle<AssetDirectory>() );
            asset->mInternal->name = std::move( pAssetName );
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
