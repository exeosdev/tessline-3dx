
#include <ts3/system/assetSystemNative.h>
#include <ts3/system/fileCommon.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

	namespace platform
	{

		int _androidTranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos );

		AAsset * _androidResolveAsset( AAssetManager * pAAssetManager,
		                               AAssetDir * pAAssetDir,
		                               FSUtilityAPI::FilePathInfo & pAssetPathInfo,
		                               Bitmask<EAssetOpenFlags> pFlags );

	}


	AndroidAssetLoader::AndroidAssetLoader( SysContextHandle pSysContext )
	: AndroidNativeObject( std::move( pSysContext ) )
	{
		_initializeAndroidAssetLoaderData();
	}

	AndroidAssetLoader::~AndroidAssetLoader() noexcept
	{
		_releaseAndroidAssetLoaderData();
	}

	void AndroidAssetLoader::_initializeAndroidAssetLoaderData()
	{
		auto & aSessionData = platform::androidGetASessionData( *this );
		mNativeData.aAssetManager = aSessionData.aCommonAppState->activity->assetManager;
	}

	void AndroidAssetLoader::_releaseAndroidAssetLoaderData()
	{
		mNativeData.aAssetManager = nullptr;
	}

	AssetHandle AndroidAssetLoader::_nativeOpenSubAsset( FSUtilityAPI::FilePathInfo pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags )
	{
		SysHandle<AndroidAsset> asset = nullptr;

		auto * aAssetManager = mNativeData.aAssetManager;

		if( auto * aAsset = platform::_androidResolveAsset( aAssetManager, nullptr, pAssetPathInfo, pFlags ) )
		{
			asset = createSysObject<AndroidAsset>( getHandle<AndroidAssetLoader>() );
			asset->setName( std::move( pAssetPathInfo.fileName ) );
			asset->mNativeData.aAsset = aAsset;
			asset->mNativeData.aAssetManager = mNativeData.aAssetManager;
		}

		return asset;
	}

	AssetDirectoryHandle AndroidAssetLoader::_nativeOpenDirectory( std::string pDirectoryName )
	{
		std::string assetDirRefName = "assets";
		assetDirRefName.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
		assetDirRefName.append( pDirectoryName );

		SysHandle<AndroidAssetDirectory> assetDirectory = nullptr;

		if( auto * aAssetDir = AAssetManager_openDir( mNativeData.aAssetManager, pDirectoryName.c_str() ) )
		{
			assetDirectory = createSysObject<AndroidAssetDirectory>( getHandle<AndroidAssetLoader>() );
			assetDirectory->setDirName( std::move( pDirectoryName ) );
			assetDirectory->mNativeData.aAssetDir = aAssetDir;
		}

		return assetDirectory;
	}

	bool AndroidAssetLoader::_nativeCheckDirectoryExists( const std::string & pDirectoryName ) const
	{
		std::string assetDirRefName = "assets";
		assetDirRefName.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
		assetDirRefName.append( pDirectoryName );

		auto * aAssetDir = AAssetManager_openDir( mNativeData.aAssetManager, assetDirRefName.c_str() );
		if( aAssetDir )
		{
			AAssetDir_close( aAssetDir );
			return true;
		}

		return false;
	}


	AndroidAssetDirectory::AndroidAssetDirectory( AndroidAssetLoaderHandle pAssetLoader )
	: AndroidNativeObject( pAssetLoader )
	{
		_initializeAndroidAssetDirectoryData( pAssetLoader->mNativeData.aAssetManager );
	}

	AndroidAssetDirectory::~AndroidAssetDirectory() noexcept
	{
		_releaseAndroidAssetDirectoryData();
	}

	void AndroidAssetDirectory::_initializeAndroidAssetDirectoryData( AAssetManager * pAssetManager )
	{
		mNativeData.aAssetManager = pAssetManager;
	}

	void AndroidAssetDirectory::_releaseAndroidAssetDirectoryData()
	{
		if( mNativeData.aAssetDir )
		{
			AAssetDir_close( mNativeData.aAssetDir );
			mNativeData.aAssetDir = nullptr;
		}
	}

	void AndroidAssetDirectory::_nativeRefreshAssetList()
	{
		FileNameList assetNameList;
		while( auto * aAssetName = AAssetDir_getNextFileName( mNativeData.aAssetDir ) )
		{
			assetNameList.push_back( std::string( aAssetName ) );
		}

		AAssetDir_rewind( mNativeData.aAssetDir );

		setAssetList( std::move( assetNameList ) );
	}

	AssetHandle AndroidAssetDirectory::_nativeOpenAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags )
	{
		SysHandle<AndroidAsset> asset = nullptr;

		FSUtilityAPI::FilePathInfo assetPathInfo;
		assetPathInfo.directory = getDirName();
		assetPathInfo.fileName = std::move( pAssetName );

		auto * aAssetManager = mNativeData.aAssetManager;
		auto * aAssetDir = mNativeData.aAssetDir;

		if( auto * aAsset = platform::_androidResolveAsset( aAssetManager, aAssetDir, assetPathInfo, pFlags ) )
		{
			asset = createSysObject<AndroidAsset>( getHandle<AndroidAssetLoader>() );
			asset->setName( std::move( assetPathInfo.fileName ) );
			asset->mNativeData.aAsset = aAsset;
			asset->mNativeData.aAssetManager = mNativeData.aAssetManager;
		}

		return asset;
	}

	bool AndroidAssetDirectory::_nativeCheckAssetExists( const std::string & pAssetName ) const
	{
		if( auto * aAsset = AAssetManager_open( mNativeData.aAssetManager, pAssetName.c_str(), AASSET_MODE_BUFFER ) )
		{
			AAsset_close( aAsset );
			return true;
		}
		return false;
	}


	AndroidAsset::AndroidAsset( AndroidAssetLoaderHandle pAssetLoader )
	: AndroidNativeObject( std::move( pAssetLoader ) )
	{}

	AndroidAsset::AndroidAsset( AndroidAssetDirectoryHandle pAssetDirectory )
	: AndroidNativeObject( std::move( pAssetDirectory ) )
	{}

	AndroidAsset::~AndroidAsset() noexcept
	{
		_releaseAndroidAssetData();
	}

	void AndroidAsset::_releaseAndroidAssetData()
	{
		if( mNativeData.aAsset )
		{
			AAsset_close( mNativeData.aAsset );
			mNativeData.aAsset = nullptr;
		}
	}

	file_size_t AndroidAsset::_nativeReadData( void * pTargetBuffer, file_size_t pReadSize )
	{
		int readResult = AAsset_read( mNativeData.aAsset, pTargetBuffer, pReadSize );
		return trunc_numeric_cast<file_size_t>( readResult );
	}

	file_offset_t AndroidAsset::_nativeSetReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
	{
		auto seekOrigin = platform::_androidTranslateFilePointerRefPos( pRefPos );
		auto seekResult = AAsset_seek64( mNativeData.aAsset, pOffset, seekOrigin );
		return trunc_numeric_cast<file_offset_t>( seekResult );
	}

	file_size_t AndroidAsset::_nativeGetSize() const
	{
		auto assetSize = AAsset_getLength64( mNativeData.aAsset );
		return trunc_numeric_cast<file_offset_t>( assetSize );
	}


	namespace platform
	{

		int _androidTranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos )
		{
			int seekOrigin = 0;

			switch( pFileRefPos )
			{
				case EFilePointerRefPos::FileBeg:
				{
					seekOrigin = SEEK_SET;
					break;
				}
				case EFilePointerRefPos::FileEnd:
				{
					seekOrigin = SEEK_END;
					break;
				}
				case EFilePointerRefPos::PtrCurrent:
				{
					seekOrigin = SEEK_CUR;
					break;
				}
			}

			return seekOrigin;
		}

		AAsset * _androidResolveAsset( AAssetManager * pAAssetManager,
		                               AAssetDir * pAAssetDir,
		                               FSUtilityAPI::FilePathInfo & pAssetPathInfo,
		                               Bitmask<EAssetOpenFlags> pFlags )
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

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
