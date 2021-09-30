
#include "assetSystemNative.h"

namespace ts3::system
{

    AssetLoader::AssetLoader( SysContextHandle pSysContext )
    : SysObject( pSysContext )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        _nativeConstructor();
    }

    AssetLoader::~AssetLoader() noexcept
    {
        _nativeDestructor();
    }

    AssetHandle AssetLoader::openSubAsset( const std::string & pAssetRefName )
    {
        if( pAssetRefName.empty() )
        {
            return nullptr;
        }

        auto assetRefName = FileUtilityAPI::normalizePath( pAssetRefName );
        auto assetPathInfo = FileUtilityAPI::splitFilePath( std::move( assetRefName ), E_FILE_API_FLAG_SPLIT_PATH_ASSUME_FILE_BIT );

        return _nativeOpenSubAsset( std::move( assetPathInfo ) );
    }

    AssetDirectoryHandle AssetLoader::openDirectory( std::string pDirectoryName )
    {
        if( pDirectoryName.empty() )
        {
            return nullptr;
        }

        auto assetDirectory = _nativeOpenDirectory( std::move( pDirectoryName ) );
        if( assetDirectory )
        {
            assetDirectory->refreshAssetList();
        }

        return assetDirectory;
    }

    bool AssetLoader::checkDirectoryExists( const std::string & pDirectoryName ) const
    {
        if( pDirectoryName.empty() )
        {
            return false;
        }
        return _nativeCheckDirectoryExists( pDirectoryName );
    }


    AssetDirectory::AssetDirectory( AssetLoaderHandle pAssetLoader )
    : SysObject( pAssetLoader->mSysContext )
    , mAssetLoader( std::move( pAssetLoader ) )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    , mDirName( mInternal->dirName )
    {
        _nativeConstructor();
    }

    AssetDirectory::~AssetDirectory() noexcept
    {
        _nativeDestructor();
    }

    void AssetDirectory::refreshAssetList()
    {
        if( !mInternal->assetNameList.empty() )
        {
            mInternal->assetNameList.clear();
        }
        _nativeRefreshAssetList();
    }

    AssetHandle AssetDirectory::openAsset( std::string pAssetName )
    {
        if( pAssetName.empty() )
        {
            return nullptr;
        }
        return _nativeOpenAsset( std::move( pAssetName ) );
    }

    const AssetNameList & AssetDirectory::getAssetList() const
    {
        return mInternal->assetNameList;
    }

    bool AssetDirectory::checkAssetExists( const std::string & pAssetName ) const
    {
        return _nativeCheckAssetExists( pAssetName );
    }


    Asset::Asset( AssetLoaderHandle pAssetLoader )
    : SysObject( pAssetLoader->mSysContext )
    , mAssetDirectory( nullptr )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    , mName( mInternal->name )
    {
        _nativeConstructor();
    }

    Asset::Asset( AssetDirectoryHandle pAssetDirectory )
    : SysObject( pAssetDirectory->mSysContext )
    , mAssetDirectory( std::move( pAssetDirectory ) )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    , mName( mInternal->name )
    {
        _nativeConstructor();
    }

    Asset::~Asset() noexcept
    {
        _nativeDestructor();
    }

    file_size_t Asset::readData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize )
    {
        return 0u;
    }

    file_size_t Asset::readData( MemoryBuffer & pBuffer, file_size_t pReadSize )
    {
        return 0u;
    }

    file_offset_t Asset::setReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return _nativeSetReadPointer( pOffset, pRefPos );
    }

    void Asset::resetReadPointer()
    {
        _nativeSetReadPointer( 0, EFilePointerRefPos::FileBeginning );
    }

} // namespace ts3::system
