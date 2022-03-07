
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
        if( !pBuffer || ( pBufferSize == 0 ) || ( pReadSize == 0 ) )
        {
            return 0;
        }

        const auto readSize = getMinOf( pBufferSize, pReadSize );

        return _nativeReadData( pBuffer, pBufferSize, readSize );
    }

    file_size_t File::readData( const ReadWriteMemoryView & pBuffer, file_size_t pReadSize )
    {
    	if( pBuffer.empty() || ( pReadSize == 0 ) )
        {
            return 0;
        }

        const auto bufferSize = pBuffer.size();
    	const auto readSize = getMinOf( bufferSize, pReadSize );

    	return _nativeReadData( pBuffer.data(), bufferSize, readSize );
    }

    file_size_t File::readData( DynamicByteArray & pBuffer, file_size_t pReadSize )
    {
        if( pReadSize == 0 )
        {
            return 0;
        }

        pBuffer.resize( pReadSize );

        return _nativeReadData( pBuffer.data(), pReadSize, pReadSize );
    }

    file_size_t File::readData( std::vector<byte> & pBuffer, file_size_t pReadSize )
    {
    	if( pReadSize == 0 )
    	{
    		return 0;
    	}

    	pBuffer.resize( pReadSize );

    	return _nativeReadData( pBuffer.data(), pReadSize, pReadSize );
    }

    file_size_t File::writeData( const void * pBuffer, file_size_t pBufferSize, file_size_t pWriteSize )
    {
        if( !pBuffer || ( pBufferSize == 0 ) || ( pWriteSize == 0 ) )
        {
            return 0;
        }

        const auto writeSize = getMinOf( pBufferSize, pWriteSize );

        return _nativeWriteData( pBuffer, pBufferSize, writeSize );
    }

    file_offset_t File::setFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return _nativeSetFilePointer( pOffset, pRefPos );
    }

    bool File::checkEOF() const
    {
        return _nativeCheckEOF();
    }

    bool File::isGood() const
    {
        return _nativeIsGood();
    }

    file_offset_t File::getFilePointer() const
    {
    	return _nativeGetFilePointer();
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
