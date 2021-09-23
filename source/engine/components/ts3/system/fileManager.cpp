
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
        auto fileListIter = mInternal->fileList.emplace( mInternal->fileList.end(), this );

        auto & fileInstance = *fileListIter;
        auto & fileObject = fileListIter->fileObject;

        mInternal->fileMap[&fileObject] = &fileInstance;

        return FileHandle{ &fileObject, FileDeleter() };
    }


    File::File( FileManager * pFileManager, ObjectInternalData * pPrivate )
    : mFileManager( pFileManager )
    , mInternal( pPrivate )
    , mName( mInternal->name )
    , mFullPath( mInternal->fullPath )
    {}

    File::~File() = default;

}
