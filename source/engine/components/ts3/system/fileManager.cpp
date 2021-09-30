
#include "fileManagerNative.h"

namespace ts3::system
{

    File::File( FileManagerHandle pFileManager )
    : SysObject( pFileManager->mSysContext )
    , mFileManager( std::move( pFileManager ) )
    , mInternal( std::make_unique<ObjectInternalData>() )
    , mName( mInternal->name )
    , mFullPath( mInternal->fullPath )
    {
        _nativeConstructor();
    }

    File::~File() noexcept
    {
        _nativeDestructor();
    }

    file_offset_t File::setFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return _nativeSetFilePointer( pOffset, pRefPos );
    }


    FileManager::FileManager( SysContextHandle pSysContext )
    : SysObject( std::move( pSysContext ) )
    {
        _nativeConstructor();
    }

    FileManager::~FileManager() noexcept
    {
        _nativeDestructor();
    }

    FileHandle FileManager::openFile( std::string pFilePath, EFileOpenMode pOpenMode )
    {
        auto fileObject = createSysObject<File>( getHandle<FileManager>() );
        _nativeOpenFile( *fileObject, std::move( pFilePath ), pOpenMode );
        return fileObject;

    }

    FileHandle FileManager::createFile( std::string pFilePath )
    {
        auto fileObject = createSysObject<File>( getHandle<FileManager>() );
        _nativeCreateFile( *fileObject, std::move( pFilePath ) );
        return fileObject;
    }

    FileHandle FileManager::createTemporaryFile()
    {
        auto fileObject = createSysObject<File>( getHandle<FileManager>() );
        _nativeCreateTemporaryFile( *fileObject );
        return fileObject;
    }

    FileList FileManager::openDirectoryFiles( const std::string & pDirectory )
    {
        FileList resultFileList;

        auto fileNameList = enumDirectoryFiles( pDirectory );

        if( !fileNameList.empty() )
        {
            resultFileList.reserve( fileNameList.size() );

            for( auto & fileName : fileNameList )
            {
                auto fileHandle = openFile( std::move( fileName ), EFileOpenMode::ReadOnly );
                resultFileList.push_back( std::move( fileHandle ) );
            }
        }

        return resultFileList;
    }

    FileNameList FileManager::enumDirectoryFiles( const std::string & pDirectory )
    {
        if( pDirectory.empty() )
        {
            return {};
        }
        return _nativeEnumDirectoryFileNameList( pDirectory );
    }

    std::string FileManager::generateTemporaryFileName()
    {
        return _nativeGenerateTemporaryFileName();
    }

    bool FileManager::checkDirectoryExists( const std::string & pDirPath )
    {
        if( pDirPath.empty() )
        {
            return false;
        }
        return _nativeCheckDirectoryExists( pDirPath );
    }

    bool FileManager::checkFileExists( const std::string & pFilePath )
    {
        if( pFilePath.empty() )
        {
            return false;
        }
        return _nativeCheckFileExists( pFilePath );
    }

}
