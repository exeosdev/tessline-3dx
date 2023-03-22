
#ifndef __TS3_SYSTEM_PLATFORM_COMMON_FILE_BASED_ASSET_SYSTEM__
#define __TS3_SYSTEM_PLATFORM_COMMON_FILE_BASED_ASSET_SYSTEM__

#include <ts3/system/assetSystem.h>
#include <ts3/system/fileCommon.h>

#if( TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT )
namespace ts3::system
{

	ts3SysDeclareHandle( FileAssetLoader );

	namespace platform
	{

		struct FileAssetNativeData
		{
			FileHandle fileHandle = nullptr;
			std::string filePath;
		};

		struct FileAssetDirectoryNativeData
		{
			std::string combinedDirPath;
		};

		struct FileAssetLoaderNativeData
		{
			std::string rootDir;
		};

		struct AssetLoaderCreateInfoNativeParams
		{
			FileManagerHandle fileManager = nullptr;
			std::string absoluteAssetRootDir;
			std::string relativeAssetRootDir;
		};

		TS3_SYSTEM_API_NODISCARD AssetLoaderHandle createFileAssetLoaderExplicit(
				SysContextHandle pSysContext,
				FileManagerHandle pFileManager,
				const std::string & pAbsoluteAssetDirectory );

		TS3_SYSTEM_API_NODISCARD AssetLoaderHandle createFileAssetLoaderResolve(
				SysContextHandle pSysContext,
				FileManagerHandle pFileManager,
				const std::string & pRelativeAssetDirectory );

		TS3_SYSTEM_API_NODISCARD AssetLoaderHandle createFileAssetLoader(
				SysContextHandle pSysContext,
				const AssetLoaderCreateInfoNativeParams & pCreateParams );

	}

	class FileAssetLoader : public NativeObject<AssetLoader, platform::FileAssetLoaderNativeData>
	{
	public:
		FileManagerHandle const mFileManager;

	public:
		explicit FileAssetLoader( FileManagerHandle pFileManager );
		virtual ~FileAssetLoader() noexcept;

		void setRootDir( std::string pRootDir );

	private:
		/// @copybrief AssetLoader::_nativeOpenSubAsset
		virtual AssetHandle _nativeOpenSubAsset( FSUtilityAPI::FilePathInfo pAssetPathInfo,
		                                         Bitmask<EAssetOpenFlags> pFlags ) override final;

		/// @copybrief AssetLoader::_nativeOpenDirectory
		virtual AssetDirectoryHandle _nativeOpenDirectory( std::string pDirectoryName ) override final;

		/// @copybrief AssetLoader::_nativeCheckDirectoryExists
		virtual bool _nativeCheckDirectoryExists( const std::string & pDirectoryName ) const override final;
	};
	
	class FileAssetDirectory : public NativeObject<AssetDirectory, platform::FileAssetDirectoryNativeData>
	{
		friend class FileAssetLoader;

	public:
		FileManagerHandle const mFileManager;

	public:
		explicit FileAssetDirectory( FileAssetLoaderHandle pAssetLoader );
		virtual ~FileAssetDirectory() noexcept;

		using AssetDirectory::addAsset;
		using AssetDirectory::setDirName;
		
	private:
		/// @copybrief AssetDirectory::_nativeRefreshAssetList
		virtual void _nativeRefreshAssetList() override final;

		/// @copybrief AssetDirectory::_nativeOpenAsset
		virtual AssetHandle _nativeOpenAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags ) override final;

		/// @copybrief AssetDirectory::_nativeCheckAssetExists
		virtual bool _nativeCheckAssetExists( const std::string & pAssetName ) const override final;
	};

	class FileAsset : public NativeObject<Asset, platform::FileAssetNativeData>
	{
		friend class FileAssetLoader;
		
	public:
		explicit FileAsset( AssetLoaderHandle pAssetLoader );
		explicit FileAsset( AssetDirectoryHandle pAssetDirectory );
		virtual ~FileAsset() noexcept;

		using Asset::setName;

	private:
		/// @copybrief Asset::_nativeReadData
		virtual file_size_t _nativeReadData( void * pTargetBuffer, file_size_t pReadSize ) override final;

		/// @copybrief Asset::_nativeSetReadPointer
		virtual file_offset_t _nativeSetReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos ) override final;

		/// @copybrief Asset::_nativeGetSize
		virtual file_size_t _nativeGetSize() const override final;
	};

} // namespace ts3::system
#endif // TS3_SYSTEM_USE_ASSET_SYSTEM_DEFAULT

#endif // __TS3_SYSTEM_PLATFORM_COMMON_FILE_BASED_ASSET_SYSTEM__
