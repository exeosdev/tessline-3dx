
#include "fileBasedAssetSystem.h"
#include <ts3/system/fileManager.h>
#include <ts3/system/sysContext.h>
#include <ts3/system/assetSystem.h>

namespace ts3::system
{

	namespace platform
	{

		std::string _commonResolveFileName( FileManager & pFileManager,
		                                    FSUtilityAPI::FilePathInfo & pAssetPathInfo,
		                                    Bitmask<EAssetOpenFlags> pFlags );

	}


	FileAssetLoader::FileAssetLoader( FileManagerHandle pFileManager )
	: NativeObject( pFileManager->mSysContext )
	, mFileManager( std::move( pFileManager ) ) 
	{}

	FileAssetLoader::~FileAssetLoader() noexcept = default;

	void FileAssetLoader::setRootDir( std::string pRootDir )
	{
		mNativeData.rootDir = std::move( pRootDir );
	}

	AssetHandle FileAssetLoader::_nativeOpenSubAsset( FSUtilityAPI::FilePathInfo pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags )
	{
		SysHandle<FileAsset> asset = nullptr;

		auto fileDirectoryPath = mNativeData.rootDir;
		fileDirectoryPath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
		fileDirectoryPath.append( pAssetPathInfo.directory );

		pAssetPathInfo.directory = std::move( fileDirectoryPath );

		if( mFileManager->checkDirectoryExists( pAssetPathInfo.directory ) )
		{
			auto combinedFilePath = platform::_commonResolveFileName( *mFileManager, pAssetPathInfo, pFlags );

			if( mFileManager->checkFileExists( combinedFilePath ) )
			{
				auto fileHandle = mFileManager->openFile( combinedFilePath, EFileOpenMode::ReadOnly );

				asset = createSysObject<FileAsset>( getHandle<FileAssetLoader>() );
				asset->setName( std::move( pAssetPathInfo.fileName ) );
				asset->mNativeData.fileHandle = fileHandle;
				asset->mNativeData.filePath = std::move( combinedFilePath );
			}
		}

		return asset;
	}

	AssetDirectoryHandle FileAssetLoader::_nativeOpenDirectory( std::string pDirectoryName )
	{
		auto combinedDirPath = mNativeData.rootDir;
		combinedDirPath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
		combinedDirPath.append( pDirectoryName );

		if( !mFileManager->checkDirectoryExists( combinedDirPath ) )
		{
			return nullptr;
		}

		auto assetDirectory = createSysObject<FileAssetDirectory>( getHandle<FileAssetLoader>() );
		assetDirectory->setDirName( std::move( pDirectoryName ) );
		assetDirectory->mNativeData.combinedDirPath = std::move( combinedDirPath );

		return assetDirectory;
	}

	bool FileAssetLoader::_nativeCheckDirectoryExists( const std::string & pDirectoryName ) const
	{
		auto combinedDirPath = mNativeData.rootDir;
		combinedDirPath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
		combinedDirPath.append( pDirectoryName );

		return mFileManager->checkDirectoryExists( combinedDirPath );
	}


	FileAssetDirectory::FileAssetDirectory( FileAssetLoaderHandle pAssetLoader )
	: NativeObject( pAssetLoader )
	, mFileManager( pAssetLoader->mFileManager )
	{}

	FileAssetDirectory::~FileAssetDirectory() noexcept = default;

	void FileAssetDirectory::_nativeRefreshAssetList()
	{
		auto assetNameList = mFileManager->enumDirectoryFiles( mNativeData.combinedDirPath );
		setAssetList( std::move( assetNameList ) );
	}

	AssetHandle FileAssetDirectory::_nativeOpenAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags )
	{
		SysHandle<FileAsset> asset = nullptr;

		FSUtilityAPI::FilePathInfo assetPathInfo;
		assetPathInfo.directory = mNativeData.combinedDirPath;
		assetPathInfo.fileName = std::move( pAssetName );

		if( mFileManager->checkDirectoryExists( mNativeData.combinedDirPath ) )
		{
			auto combinedFilePath = platform::_commonResolveFileName( *mFileManager, assetPathInfo, pFlags );

			if( mFileManager->checkFileExists( combinedFilePath ) )
			{
				auto fileHandle = mFileManager->openFile( combinedFilePath, EFileOpenMode::ReadOnly );

				asset = createSysObject<FileAsset>( getHandle<FileAssetDirectory>() );
				asset->setName( std::move( assetPathInfo.fileName ) );
				asset->mNativeData.fileHandle = fileHandle;
				asset->mNativeData.filePath = std::move( combinedFilePath );
			}
		}

		return asset;
	}

	bool FileAssetDirectory::_nativeCheckAssetExists( const std::string & pAssetName ) const
	{
		auto combinedFilePath = mNativeData.combinedDirPath;
		combinedFilePath.append( pAssetName );
		return mFileManager->checkFileExists( combinedFilePath );
	}


	FileAsset::FileAsset( AssetLoaderHandle pAssetLoader )
	: NativeObject( std::move( pAssetLoader ) )
	{}

	FileAsset::FileAsset( AssetDirectoryHandle pAssetDirectory )
	: NativeObject( std::move( pAssetDirectory ) )
	{}

	FileAsset::~FileAsset() noexcept = default;

	file_size_t FileAsset::_nativeReadData( void * pTargetBuffer, file_size_t pReadSize )
	{
		return mNativeData.fileHandle->read( pTargetBuffer, pReadSize );
	}

	file_offset_t FileAsset::_nativeSetReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
	{
		return mNativeData.fileHandle->setFilePointer( pOffset, pRefPos );
	}

	file_size_t FileAsset::_nativeGetSize() const
	{
		return mNativeData.fileHandle->getSize();
	}


	namespace platform
	{

		AssetLoaderHandle createFileAssetLoaderExplicit(
				SysContextHandle pSysContext,
				FileManagerHandle pFileManager,
				const std::string & pAbsoluteAssetDirectory )
		{
			if( !pFileManager )
			{
				pFileManager = pSysContext->createFileManager();
			}

			auto assetDirectory = FSUtilityAPI::normalizePath( pAbsoluteAssetDirectory );
			auto assetLoader = createSysObject<FileAssetLoader>( pFileManager );
			assetLoader->setRootDir( std::move( assetDirectory ) );

			return assetLoader;
		}

		AssetLoaderHandle createFileAssetLoaderResolve(
				SysContextHandle pSysContext,
				FileManagerHandle pFileManager,
				const std::string & pRelativeAssetDirectory )
		{
			if( !pFileManager )
			{
				pFileManager = pSysContext->createFileManager();
			}

			auto assetDirectory = pSysContext->queryCurrentProcessWorkingDirectory();
			// auto assetDirectory = pSysContext->queryCurrentProcessExecutableDirectory();
			assetDirectory.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
			assetDirectory.append( pRelativeAssetDirectory );
			assetDirectory = FSUtilityAPI::normalizePath( assetDirectory );

			auto assetLoader = createSysObject<FileAssetLoader>( pFileManager );
			assetLoader->setRootDir( std::move( assetDirectory ) );

			return assetLoader;
		}

		AssetLoaderHandle createFileAssetLoader( SysContextHandle pSysContext, const AssetLoaderCreateInfoNativeParams & pCreateParams )
		{
			const auto & fileManager = pCreateParams.fileManager;

			if( !pCreateParams.absoluteAssetRootDir.empty() )
			{
				return createFileAssetLoaderExplicit( std::move( pSysContext ), fileManager, pCreateParams.absoluteAssetRootDir );
			}
			else
			{
				return createFileAssetLoaderResolve( std::move( pSysContext ), fileManager, pCreateParams.relativeAssetRootDir );
			}
		}


		std::string _commonResolveFileName( FileManager & pFileManager,
		                                    FSUtilityAPI::FilePathInfo & pAssetPathInfo,
		                                    Bitmask<EAssetOpenFlags> pFlags )
		{
			auto combinedFilePath = pAssetPathInfo.directory;
			combinedFilePath.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
			combinedFilePath.append( pAssetPathInfo.fileName );

			if( !pFileManager.checkFileExists( combinedFilePath ) )
			{
				combinedFilePath.clear();

				if( pFlags.isSet( E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT ) )
				{
					auto fileNameList = pFileManager.enumDirectoryFiles( pAssetPathInfo.directory );
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

}
