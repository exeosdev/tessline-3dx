
#ifndef __TS3_SYSTEM_ASSET_SYSTEM_H__
#define __TS3_SYSTEM_ASSET_SYSTEM_H__

#include "assetCommon.h"
#include "fileCommon.h"
#include "sysObject.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3::system
{

    ts3SysDeclareHandle( Asset );
    ts3SysDeclareHandle( AssetDirectory );
    ts3SysDeclareHandle( AssetLoader );

    struct AssetNativeData;
    struct AssetDirectoryNativeData;
    struct AssetLoaderNativeData;

    using AssetNameList = std::vector<std::string>;

    enum EAssetOpenFlags : uint32
    {
        E_ASSET_OPEN_FLAG_NO_EXTENSION_BIT = 0x0001
    };

    class AssetLoader : public SysObject
    {
    public:
        struct ObjectInternalData;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const AssetLoaderNativeData * const mNativeData;

    public:
        explicit AssetLoader( SysContextHandle pSysContext );
        virtual ~AssetLoader() noexcept;

        AssetHandle openSubAsset( const std::string & pAssetRefName, Bitmask<EAssetOpenFlags> pFlags = 0u );

        AssetDirectoryHandle openDirectory( std::string pDirectoryName );

        bool checkDirectoryExists( const std::string & pDirectoryName ) const;

    private:
        void _nativeConstructor();
        void _nativeDestructor() noexcept;
        AssetHandle _nativeOpenSubAsset( FileUtilityAPI::FilePathInfo pAssetPathInfo, Bitmask<EAssetOpenFlags> pFlags );
        AssetDirectoryHandle _nativeOpenDirectory( std::string pDirectoryName );
        bool _nativeCheckDirectoryExists( const std::string & pDirectoryName ) const;
    };

    class AssetDirectory : public SysObject
    {
    public:
        struct ObjectInternalData;
        AssetLoaderHandle const mAssetLoader;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const AssetDirectoryNativeData * const mNativeData;
        const std::string & mDirName;

    public:
        explicit AssetDirectory( AssetLoaderHandle pAssetLoader );
        virtual ~AssetDirectory() noexcept;

        void refreshAssetList();

        AssetHandle openAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags = 0u );

        const AssetNameList & getAssetList() const;

        bool checkAssetExists( const std::string & pAssetName ) const;

    private:
        void _nativeConstructor();
        void _nativeDestructor() noexcept;
        void _nativeRefreshAssetList();
        AssetHandle _nativeOpenAsset( std::string pAssetName, Bitmask<EAssetOpenFlags> pFlags );
        bool _nativeCheckAssetExists( const std::string & pAssetName ) const;
    };

    class Asset : public SysObject
    {
    public:
        struct ObjectInternalData;
        AssetDirectoryHandle const mAssetDirectory;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const AssetNativeData * const mNativeData;
        const std::string & mName;

    public:
        explicit Asset( AssetLoaderHandle pAssetLoader );
        explicit Asset( AssetDirectoryHandle pAssetDirectory );
        virtual ~Asset() noexcept;

        file_size_t readData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize = CX_FILE_SIZE_MAX );
        file_size_t readData( MemoryBuffer & pBuffer, file_size_t pReadSize = CX_FILE_SIZE_MAX );

        template <typename TpChar>
        file_size_t readData( std::basic_string<TpChar> & pString, file_size_t pReadSize = CX_FILE_SIZE_MAX )
        {
            return readData( pString.data(), pString.length() * sizeof( TpChar ), pReadSize );
        }

        template <typename TpValue>
        file_size_t readData( std::vector<TpValue> & pVector, file_size_t pReadSize = CX_FILE_SIZE_MAX )
        {
            return readData( pVector.data(), pVector.size() * sizeof( TpValue ), pReadSize );
        }

        file_size_t readAll( DynamicMemoryBuffer & pBuffer )
        {
            const auto assetSize = _nativeGetSize();
            pBuffer.resize( assetSize );
            return readData( pBuffer.dataPtr(), assetSize, assetSize );
        }

        template <typename TpChar>
        file_size_t readAll( std::basic_string<TpChar> & pString )
        {
            const auto assetSize = _nativeGetSize();
            const auto strLength = assetSize / sizeof( TpChar );
            pString.resize( strLength + 1 );
            return readData( pString.data(), pString.length() * sizeof( TpChar ), assetSize );
        }

        template <typename TpValue>
        file_size_t readAll( std::vector<TpValue> & pVector )
        {
            const auto assetSize = _nativeGetSize();
            const auto vectorSize = assetSize / sizeof( TpValue );
            pVector.resize( vectorSize );
            return readData( pVector.data(), pVector.size() * sizeof( TpValue ), vectorSize );
        }

        file_offset_t setReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos = EFilePointerRefPos::FileBeginning );

        void resetReadPointer();

    private:
        void _nativeConstructor();
        void _nativeDestructor() noexcept;
        file_size_t _nativeReadData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize );
        file_offset_t _nativeSetReadPointer( file_offset_t pOffset, EFilePointerRefPos pRefPos );
        file_size_t _nativeGetSize() const;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_ASSET_SYSTEM_H__
