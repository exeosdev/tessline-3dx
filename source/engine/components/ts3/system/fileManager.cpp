
#include "fileManagerNative.h"

namespace ts3::system
{

    FileHandle FileManager::openFile( std::string pFilePath, EFileOpenMode pOpenMode )
    {
        auto fileHandle = _createFileInstance();
        return fileHandle;

    }

    FileHandle FileManager::createFile( std::string pFilePath )
    {
        auto fileHandle = _createFileInstance();
        return fileHandle;
    }

    FileHandle FileManager::createTemporaryFile()
    {
        auto fileHandle = _createFileInstance();
        return fileHandle;
    }

    FileHandle FileManager::_createFileInstance()
    {
        auto fileListIter = mPrivate->fileList.emplace( mPrivate->fileList.end(), this );

        auto & fileInstance = *fileListIter;
        auto & fileObject = fileListIter->fileObject;

        mPrivate->fileMap[&fileObject] = &fileInstance;

        return FileHandle{ &fileObject, FileDeleter() };
    }


    File::File( FileManager * pFileManager, ObjectPrivateData * pPrivate )
    : mFileManager( pFileManager )
    , mPrivate( pPrivate )
    , mName( mPrivate->name )
    , mFullPath( mPrivate->fullPath )
    {}

    File::~File() = default;

}
