
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_MANAGER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_MANAGER_H__

#include <ts3/system/fileManager.h>
#include "win32FileAPI.h"

namespace ts3::system
{

    namespace platform
    {

        struct Win32FileNativeData
        {
            HANDLE fileHandle = nullptr;
        };

    }

    class Win32FileManager : public Win32NativeObject<FileManager, void>
    {
    public:
        explicit Win32FileManager( SysContextHandle pSysContext );
        virtual ~Win32FileManager() noexcept;

    private:
        virtual FileHandle _nativeOpenFile( std::string pFilePath, EFileOpenMode pOpenMode ) override final;
        virtual FileHandle _nativeCreateFile( std::string pFilePath ) override final;
        virtual FileHandle _nativeCreateTemporaryFile() override final;
        virtual FileNameList _nativeEnumDirectoryFileNameList( const std::string & pDirectory ) override final;
        virtual std::string _nativeGenerateTemporaryFileName() override final;
        virtual bool _nativeCheckDirectoryExists( const std::string & pDirPath ) override final;
        virtual bool _nativeCheckFileExists( const std::string & pFilePath ) override final;
    };

    class Win32File : public Win32NativeObject<File, platform::Win32FileNativeData>
    {
        friend class Win32FileManager;

    public:
        explicit Win32File( FileManagerHandle pFileManager );
        virtual ~Win32File() noexcept;

    friendapi:
        void setInternalWin32FileHandle( HANDLE pFileHandle );

    private:
        void _releaseWin32FileHandle();

        virtual file_size_t _nativeReadData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize ) override final;
        virtual file_offset_t _nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos ) override final;
        virtual file_size_t _nativeGetSize() const override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_MANAGER_H__
