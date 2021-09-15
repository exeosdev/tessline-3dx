
#ifndef __TS3_SYSTEM_FILE_MANAGER_H__
#define __TS3_SYSTEM_FILE_MANAGER_H__

#include "fileCommon.h"
#include "sysObject.h"

namespace ts3::system
{

    class File
    {
    public:
        struct ObjectPrivateData;
        FileManager * const mFileManager = nullptr;
        ObjectPrivateData * const mPrivate = nullptr;
        const file_str_t & mName;
        const file_str_t & mFullPath;

    public:
        File( FileManager * pFileManager, ObjectPrivateData * pPrivate );
        ~File();

    private:
        void _release();
    };

    class FileManager : public SysObject
    {
        friend struct FileDeleter;

    public:
        struct ObjectPrivateData;
        std::unique_ptr<ObjectPrivateData> const mPrivate;

    public:
        explicit FileManager( SysContext pSysContext );
        ~FileManager();

        FileHandle openFile( std::string pFilePath, EFileOpenMode pOpenMode );
        FileHandle createFile( std::string pFilePath );
        FileHandle createTemporaryFile();

        void renameFile( const std::string & pFileName, const std::string & pNewName );

        std::string generateTemporaryFileName();

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
