
#ifndef __TS3_SYSTEM_FILE_MANAGER_H__
#define __TS3_SYSTEM_FILE_MANAGER_H__

#include "fileCommon.h"
#include "sysObject.h"

namespace ts3::system
{

    using FileList = std::vector<FileHandle>;
    using FileNameList = std::vector<std::string>;

    class File : public SysObject
    {
    public:
        struct ObjectInternalData;
        FileManagerHandle mFileManager = nullptr;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const file_str_t & mName;
        const file_str_t & mFullPath;

    public:
        File( FileManagerHandle pFileManager );
        ~File() noexcept;

        file_offset_t setFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos = EFilePointerRefPos::FileBeginning );

    private:
        void _nativeConstructor();
        void _nativeDestructor() noexcept;
        file_offset_t _nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos );
    };

    class FileManager : public SysObject
    {
        friend struct FileDeleter;

    public:
        struct ObjectInternalData;
        std::unique_ptr<ObjectInternalData> const mInternal;

    public:
        explicit FileManager( SysContextHandle pSysContext );
        ~FileManager() noexcept;

        FileHandle openFile( std::string pFilePath, EFileOpenMode pOpenMode );

        FileHandle createFile( std::string pFilePath );

        FileHandle createTemporaryFile();

        FileList openDirectoryFiles( const std::string & pDirectory );

        static FileNameList enumDirectoryFiles( const std::string & pDirectory );

        static std::string generateTemporaryFileName();

        static bool checkDirectoryExists( const std::string & pDirPath );

        static bool checkFileExists( const std::string & pFilePath );

    private:
        void _nativeConstructor();
        void _nativeDestructor() noexcept;

        static void _nativeOpenFile( File & pFile, std::string pFilePath, EFileOpenMode pOpenMode );
        static void _nativeCreateFile( File & pFile, std::string pFilePath );
        static void _nativeCreateTemporaryFile( File & pFile );
        static FileNameList _nativeEnumDirectoryFileNameList( const std::string & pDirectory );
        static std::string _nativeGenerateTemporaryFileName();
        static bool _nativeCheckDirectoryExists( const std::string & pDirPath );
        static bool _nativeCheckFileExists( const std::string & pFilePath );
    };

}

#endif //__TS3_SYSTEM_FILE_MANAGER_H__
