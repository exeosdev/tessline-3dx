
#include "internal/assetSystemPrivate.h"

namespace ts3::system
{

    AssetLoader::AssetLoader( SysContextHandle pSysContext )
    : SysObject( pSysContext )
    {}

    AssetLoader::~AssetLoader() noexcept = default;

    AssetHandle AssetLoader::openSubAsset( const std::string & pAssetRefName, Bitmask<EAssetOpenFlags> pFlags )
    {
        if( pAssetRefName.empty() )
        {
            return nullptr;
        }

        auto assetRefName = FSUtilityAPI::normalizePath( pAssetRefName );
        auto assetPathInfo = FSUtilityAPI::splitPath( std::move( assetRefName ), FSUtilityAPI::E_SPLIT_PATH_FLAG_ASSUME_FILE_BIT );

        return _nativeOpenSubAsset( std::move( assetPathInfo ), pFlags );
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
    , _privateData( std::make_unique<AssetDirectoryPrivateData>() )
    {}

    AssetDirectory::~AssetDirectory() noexcept = default;

    void AssetDirectory::refreshAssetList()
    {
        if( !_privateData->assetNameList.empty() )
        {
            _privateData->assetNameList.clear();
        }
        _nativeRefreshAssetList();
    }

    AssetHandle AssetDirectory::openAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags )
    {
        if( pAssetName.empty() )
        {
            return nullptr;
        }
        return _nativeOpenAsset( std::move( pAssetName ), pFlags );
    }

    const AssetNameList & AssetDirectory::getAssetList() const
    {
        return _privateData->assetNameList;
    }

    bool AssetDirectory::checkAssetExists( const std::string & pAssetName ) const
    {
        return _nativeCheckAssetExists( pAssetName );
    }

    const std::string & AssetDirectory::getDirName() const
    {
        return _privateData->dirName;
    }

    void AssetDirectory::addAsset( std::string pAssetName )
    {
        _privateData->assetNameList.push_back( std::move( pAssetName ) );
    }

    void AssetDirectory::setAssetList( AssetNameList pAssetList )
    {
        _privateData->assetNameList = std::move( pAssetList );
    }

    void AssetDirectory::setDirName( std::string pDirName )
    {
        _privateData->dirName = std::move( pDirName );
    }


    Asset::Asset( AssetLoaderHandle pAssetLoader )
    : SysObject( pAssetLoader->mSysContext )
    , mAssetDirectory( nullptr )
    , _privateData( std::make_unique<AssetPrivateData>() )
    {}

    Asset::Asset( AssetDirectoryHandle pAssetDirectory )
    : SysObject( pAssetDirectory->mSysContext )
    , mAssetDirectory( std::move( pAssetDirectory ) )
    , _privateData( std::make_unique<AssetPrivateData>() )
    {}

    Asset::~Asset() noexcept = default;

    file_size_t Asset::readData( void * pTargetBuffer, file_size_t pTargetBufferSize, file_size_t pReadSize )
    {
    	if( !pTargetBuffer || ( pTargetBufferSize == 0 ) || ( pReadSize == 0 ) )
    	{
    		return 0;
    	}

    	const auto readSize = getMinOf( pTargetBufferSize, pReadSize );

    	return _nativeReadData( pTargetBuffer, readSize );
    }

    file_size_t Asset::readData( MemoryBuffer & pBuffer, file_size_t pReadSize )
    {
    	return readData( pBuffer.data(), pBuffer.size(), pReadSize );
    }

    file_offset_t Asset::setReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return _nativeSetReadPointer( pOffset, pRefPos );
    }

    void Asset::resetReadPointer()
    {
        _nativeSetReadPointer( 0, EFilePointerRefPos::FileBeg );
    }

    const std::string & Asset::getName() const
    {
        return _privateData->name;
    }

    void Asset::setName( std::string pAssetName )
    {
        _privateData->name = std::move( pAssetName );
    }

} // namespace ts3::system
