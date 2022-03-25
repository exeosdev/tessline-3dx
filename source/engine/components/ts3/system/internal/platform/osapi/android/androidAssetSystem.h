
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_ASSET_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_ASSET_SYSTEM_H__

#include "androidCommon.h"
#include <ts3/system/assetSystem.h>
#include <android/asset_manager.h>

namespace ts3::system
{

    ts3DeclareClassHandle( AndroidAssetLoader );
    ts3DeclareClassHandle( AndroidAssetDirectory );
    ts3DeclareClassHandle( AndroidAsset );

    namespace platform
    {

        struct AndroidAssetNativeData : public AndroidNativeDataCommon
        {
            AAssetManager * aAssetManager = nullptr;
            AAsset * aAsset = nullptr;
        };

        struct AndroidAssetDirectoryNativeData : public AndroidNativeDataCommon
        {
            AAssetManager * aAssetManager = nullptr;
            AAssetDir * aAssetDir = nullptr;
        };

        struct AndroidAssetLoaderNativeData : public AndroidNativeDataCommon
        {
            AAssetManager * aAssetManager = nullptr;
        };

        struct AssetLoaderCreateInfoNativeParams
        {
        };

    }

    class AndroidAssetLoader : public AndroidNativeObject<AssetLoader, platform::AndroidAssetLoaderNativeData>
    {
    public:
        explicit AndroidAssetLoader( SysContextHandle pSysContext );
        virtual ~AndroidAssetLoader() noexcept;

    private:
        void _initializeAndroidAssetLoaderData();
        void _releaseAndroidAssetLoaderData();

        /// @override AssetLoader::_nativeOpenSubAsset
        virtual AssetHandle _nativeOpenSubAsset( FSUtilityAPI::FilePathInfo pAssetPathInfo,
                                                 Bitmask<EAssetOpenFlags> pFlags ) override final;

        /// @override AssetLoader::_nativeOpenDirectory
        virtual AssetDirectoryHandle _nativeOpenDirectory( std::string pDirectoryName ) override final;

        /// @override AssetLoader::_nativeCheckDirectoryExists
        virtual bool _nativeCheckDirectoryExists( const std::string & pDirectoryName ) const override final;
    };

    class AndroidAssetDirectory : public AndroidNativeObject<AssetDirectory, platform::AndroidAssetDirectoryNativeData>
    {
        friend class AndroidAssetLoader;

    public:
        FileManagerHandle const mFileManager;

    public:
        explicit AndroidAssetDirectory( AndroidAssetLoaderHandle pAssetLoader );
        virtual ~AndroidAssetDirectory() noexcept;

        using AssetDirectory::addAsset;
        using AssetDirectory::setDirName;

    private:
        void _initializeAndroidAssetDirectoryData( AAssetManager * pAssetManager );
        void _releaseAndroidAssetDirectoryData();

        /// @override AssetDirectory::_nativeRefreshAssetList
        virtual void _nativeRefreshAssetList() override final;

        /// @override AssetDirectory::_nativeOpenAsset
        virtual AssetHandle _nativeOpenAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags ) override final;

        /// @override AssetDirectory::_nativeCheckAssetExists
        virtual bool _nativeCheckAssetExists( const std::string & pAssetName ) const override final;
    };

    class AndroidAsset : public AndroidNativeObject<Asset, platform::AndroidAssetNativeData>
    {
        friend class AndroidAssetLoader;

    public:
        explicit AndroidAsset( AndroidAssetLoaderHandle pAssetLoader );
        explicit AndroidAsset( AndroidAssetDirectoryHandle pAssetDirectory );
        virtual ~AndroidAsset() noexcept;

        using Asset::setName;

    private:
        void _releaseAndroidAssetData();

        /// @override Asset::_nativeReadData
        virtual file_size_t _nativeReadData( void * pTargetBuffer, file_size_t pReadSize ) override final;

        /// @override Asset::_nativeSetReadPointer
        virtual file_offset_t _nativeSetReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos ) override final;

        /// @override Asset::_nativeGetSize
        virtual file_size_t _nativeGetSize() const override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_ASSET_SYSTEM_H__
