
#ifndef __TS3_SYSTEM_FILE_MANAGER_H__
#define __TS3_SYSTEM_FILE_MANAGER_H__

#include "fileCommon.h"
#include "sysObject.h"

namespace ts3::system
{

    class File
    {
    public:
        struct ObjectInternalData;
        FileManager * const mFileManager = nullptr;
        ObjectInternalData * const mInternal = nullptr;
        const file_str_t & mName;
        const file_str_t & mFullPath;

    public:
        File( FileManager * pFileManager, ObjectInternalData * pPrivate );
        ~File();

    private:
        void _release();
    };

    class FileManager : public SysObject
    {
        friend struct FileDeleter;

    public:
        struct ObjectInternalData;
        std::unique_ptr<ObjectInternalData> const mInternal;

    public:
        explicit FileManager( SysContext pSysContext );
        ~FileManager();

        FileHandle openFile( std::string pFilePath, EFileOpenMode pOpenMode );
        FileHandle createFile( std::string pFilePath );
        FileHandle createTemporaryFile();

        void renameFile( const std::string & pFileName, const std::string & pNewName );

        std::string generateTemporaryFileName();

        bool checkDirectoryExists( const std::string & pDirPath );

    private:
        FileHandle _createFileInstance();
        void _releaseFile( File & pFile );

        void _nativeOpenFile( File & pFile, std::string pFilePath, EFileOpenMode pOpenMode );
        void _nativeCreateFile( File & pFile, std::string pFilePath );
        void _nativeCreateTemporaryFile( File & pFile );
        void _nativeCloseFile( File & pFile );
        void _nativeGenerateTemporaryFileName( std::string & pOutName );
    };

}

#endif //__TS3_SYSTEM_FILE_MANAGER_H__
