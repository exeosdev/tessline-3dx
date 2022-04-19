
#include "fileManager.h"

namespace ts3::system
{

	File::File( FileManagerHandle pFileManager )
	: SysObject( pFileManager->mSysContext )
	, mFileManager( std::move( pFileManager ) )
	{}

	File::~File() noexcept = default;

	file_size_t File::read( void * pTargetBuffer, file_size_t pTargetBufferSize, file_size_t pReadSize )
	{
		if( !pTargetBuffer || ( pTargetBufferSize == 0 ) || ( pReadSize == 0 ) )
		{
			return 0;
		}

		const auto readSize = getMinOf( pTargetBufferSize, pReadSize );

		return _nativeReadData( pTargetBuffer, readSize );
	}

	file_size_t File::read( const ReadWriteMemoryView & pTarget, file_size_t pReadSize )
	{
		return read( pTarget.data(), pTarget.size(), pReadSize );
	}

	file_size_t File::readAuto( DynamicMemoryBuffer & pTarget, file_size_t pReadSize )
	{
		return _readAuto( pTarget, pReadSize );
	}

	file_size_t File::readAuto( DynamicByteArray & pTarget, file_size_t pReadSize )
	{
		return _readAuto( pTarget, pReadSize );
	}

	file_size_t File::write( const void * pData, file_size_t pDataSize, file_size_t pWriteSize )
	{
		if( !pData || ( pDataSize == 0 ) || ( pWriteSize == 0 ) )
		{
			return 0;
		}

		const auto writeSize = getMinOf( pDataSize, pWriteSize );

		return _nativeWriteData( pData, writeSize );
	}
	
	file_size_t File::write( const MemoryBuffer & pSource, file_size_t pWriteSize )
	{
		return _write( pSource, pWriteSize );
	}

	file_size_t File::write( const ByteArray & pSource, file_size_t pWriteSize )
	{
		return _write( pSource, pWriteSize );
	}

	file_offset_t File::setFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
	{
		return _nativeSetFilePointer( pOffset, pRefPos );
	}

	file_offset_t File::getFilePointer() const
	{
		return _nativeGetFilePointer();
	}

	file_size_t File::getSize() const
	{
		return _nativeGetSize();
	}

	file_size_t File::getRemainingBytes() const
	{
		return _nativeGetRemainingBytes();
	}

	bool File::checkEOF() const
	{
		return _nativeCheckEOF();
	}

	bool File::isGood() const
	{
		return _nativeIsGood();
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
