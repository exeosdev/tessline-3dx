
#ifndef __TS3_SYSTEM_FILE_MANAGER_H__
#define __TS3_SYSTEM_FILE_MANAGER_H__

#include "fileCommon.h"
#include "sysObject.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3::system
{

    using FileList = std::vector<FileHandle>;
    using FileNameList = std::vector<std::string>;

    class FileManager : public SysObject
    {
    public:
        explicit FileManager( SysContextHandle pSysContext );
        virtual ~FileManager() noexcept;

        FileHandle openFile( std::string pFilePath, EFileOpenMode pOpenMode );

        FileHandle createFile( std::string pFilePath );

        FileHandle createTemporaryFile();

        FileList openDirectoryFiles( const std::string & pDirectory );

        FileNameList enumDirectoryFiles( const std::string & pDirectory );

        std::string generateTemporaryFileName();

        bool checkDirectoryExists( const std::string & pDirPath );

        bool checkFileExists( const std::string & pFilePath );

    private:
        virtual FileHandle _nativeOpenFile( std::string pFilePath, EFileOpenMode pOpenMode ) = 0;
        virtual FileHandle _nativeCreateFile( std::string pFilePath ) = 0;
        virtual FileHandle _nativeCreateTemporaryFile() = 0;
        virtual FileNameList _nativeEnumDirectoryFileNameList( const std::string & pDirectory ) = 0;
        virtual std::string _nativeGenerateTemporaryFileName() = 0;
        virtual bool _nativeCheckDirectoryExists( const std::string & pDirPath ) = 0;
        virtual bool _nativeCheckFileExists( const std::string & pFilePath ) = 0;
    };

    class File : public SysObject
    {
    public:
        FileManagerHandle const mFileManager;

    public:
        explicit File( FileManagerHandle pFileManager );
        virtual ~File() noexcept;

        file_size_t readData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize = CX_FILE_SIZE_MAX );
        file_size_t readData( MemoryBuffer & pBuffer, file_size_t pReadSize = CX_FILE_SIZE_MAX );

        file_offset_t setFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos = EFilePointerRefPos::FileBeginning );

        file_size_t getSize() const;

    private:
        virtual file_size_t _nativeReadData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize ) = 0;
        virtual file_offset_t _nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos ) = 0;
        virtual file_size_t _nativeGetSize() const = 0;
    };

}

#endif //__TS3_SYSTEM_FILE_MANAGER_H__
