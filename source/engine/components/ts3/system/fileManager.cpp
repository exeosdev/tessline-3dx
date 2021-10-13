
#include "fileManager.h"

namespace ts3::system
{

    File::File( FileManagerHandle pFileManager )
    : SysObject( pFileManager->mSysContext )
    , mFileManager( std::move( pFileManager ) )
    {}

    File::~File() noexcept = default;

    file_size_t File::readData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize )
    {
        if( !pBuffer || ( pBufferSize == 0 ) )
        {
            return 0;
        }

        return _nativeReadData( pBuffer, pBufferSize, getMinOf( pBufferSize, pReadSize ) );
    }

    file_size_t File::readData( MemoryBuffer & pBuffer, file_size_t pReadSize )
    {
        if( pBuffer.empty() )
        {
            return 0;
        }

        auto bufferSize = pBuffer.size();
        return _nativeReadData( pBuffer.dataPtr(), bufferSize, getMinOf( bufferSize, pReadSize ) );
    }

    file_offset_t File::setFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return _nativeSetFilePointer( pOffset, pRefPos );
    }

    file_size_t File::getSize() const
    {
        return _nativeGetSize();
    }


    FileManager::FileManager( SysContextHandle pSysContext )
    : SysObject( std::move( pSysContext ) )
    {}

    FileManager::~FileManager() noexcept = default;

    FileHandle FileManager::openFile( std::string pFilePath, EFileOpenMode pOpenMode )
    {
        return _nativeOpenFile( std::move( pFilePath ), pOpenMode );

    }

    FileHandle FileManager::createFile( std::string pFilePath )
    {
        return _nativeCreateFile( std::move( pFilePath ) );
    }

    FileHandle FileManager::createTemporaryFile()
    {
        return _nativeCreateTemporaryFile();
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
