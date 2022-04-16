
#include "posixFileManager.h"
#include <dirent.h>
#include <sys/stat.h>

namespace ts3::system
{

	namespace platform
	{

		FILE * _posixOpenFileGeneric( const char * pFilePath, const char * pOpenMode );

		const char * _posixTranslateFileOpenMode( EFileOpenMode pOpenMode );

		int _posixTranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos );

		std::string _posixGenerateTempFileName();

	}
	
	PosixFileManager::PosixFileManager( SysContextHandle pSysContext )
	: FileManager( std::move( pSysContext ) )
	{}

	PosixFileManager::~PosixFileManager() noexcept = default;

	FileHandle PosixFileManager::_nativeOpenFile( std::string pFilePath, EFileOpenMode pOpenMode )
	{
		auto openMode = platform::_posixTranslateFileOpenMode( pOpenMode );
		auto filePtr = platform::_posixOpenFileGeneric( pFilePath.c_str(), openMode );
		auto fileObject = createSysObject<PosixFile>( getHandle<PosixFileManager>() );
		fileObject->setInternalFilePtr( filePtr );

		return fileObject;
	}

	FileHandle PosixFileManager::_nativeCreateFile( std::string pFilePath )
	{
		auto filePtr = platform::_posixOpenFileGeneric( pFilePath.c_str(), "w+" );
		auto fileObject = createSysObject<PosixFile>( getHandle<PosixFileManager>() );
		fileObject->setInternalFilePtr( filePtr );

		return fileObject;
	}

	FileHandle PosixFileManager::_nativeCreateTemporaryFile()
	{
		auto tempFilePath = platform::_posixGenerateTempFileName();

		auto filePtr = platform::_posixOpenFileGeneric( tempFilePath.c_str(), "w+" );
		auto fileObject = createSysObject<PosixFile>( getHandle<PosixFileManager>() );
		fileObject->setInternalFilePtr( filePtr );

		return fileObject;
	}

	FileNameList PosixFileManager::_nativeEnumDirectoryFileNameList( const std::string & pDirectory )
	{
		FileNameList resultList;

		if( auto * dirPtr = ::opendir( pDirectory.c_str() ) )
		{
			for( auto * dirEntry = ::readdir( dirPtr ); dirEntry; )
			{
				if( ( strcmp( dirEntry->d_name, "." ) != 0 ) && ( strcmp( dirEntry->d_name, ".." ) != 0 ) )
				{
					if( dirEntry->d_type != DT_UNKNOWN )
					{
						if( ( dirEntry->d_type == DT_REG ) || ( dirEntry->d_type == DT_LNK ) )
						{
							resultList.push_back( std::string( dirEntry->d_name ) );
						}
					}
					else
					{
						const std::string filePath = pDirectory + TS3_PCL_ENV_DEFAULT_PATH_DELIMITER + dirEntry->d_name;

						struct stat statInfo;
						auto statResult = ::stat( filePath.c_str(), &statInfo );

						if( statResult == 0 )
						{
							auto fileMode = ( statInfo.st_mode & S_IFMT );
							if( ( fileMode == S_IFREG ) || ( fileMode == S_IFLNK ) )
							{
								resultList.push_back( std::string( dirEntry->d_name ) );
							}
						}
					}
				}
				dirEntry = ::readdir( dirPtr );
			}
			::closedir( dirPtr );
		}

		return resultList;
	}

	std::string PosixFileManager::_nativeGenerateTemporaryFileName()
	{
		return platform::_posixGenerateTempFileName();
	}

	bool PosixFileManager::_nativeCheckDirectoryExists( const std::string & pDirPath )
	{
		struct stat statInfo;
		auto statResult = ::stat( pDirPath.c_str(), &statInfo );
		return ( statResult == 0 ) && ( ( statInfo.st_mode & S_IFMT ) == S_IFDIR );
	}

	bool PosixFileManager::_nativeCheckFileExists( const std::string & pFilePath )
	{
		struct stat statInfo;
		auto statResult = ::stat( pFilePath.c_str(), &statInfo );
		return ( statResult == 0 ) && ( ( statInfo.st_mode & S_IFMT ) != S_IFDIR );
	}


	PosixFile::PosixFile( FileManagerHandle pFileManager )
	: NativeObject( std::move( pFileManager ) )
	{}

	PosixFile::~PosixFile() noexcept
	{
		_releasePosixFileHandle();
	}

	void PosixFile::setInternalFilePtr( FILE * pFilePtr )
	{
		ts3DebugAssert( !mNativeData.filePtr );
		mNativeData.filePtr = pFilePtr;
	}

	void PosixFile::_releasePosixFileHandle()
	{
		if( mNativeData.filePtr )
		{
			::fclose( mNativeData.filePtr );
			mNativeData.filePtr = nullptr;
		}
	}

	file_size_t PosixFile::_nativeReadData( void * pTargetBuffer, file_size_t pReadSize )
	{
		auto readBytesNum = ::fread( pTargetBuffer, 1, pReadSize, mNativeData.filePtr );
		return trunc_numeric_cast<file_size_t>( readBytesNum );
	}

	file_size_t PosixFile::_nativeWriteData( const void * pData, file_size_t pWriteSize )
	{
		auto writtenBytesNum = ::fwrite( pData, 1, pWriteSize, mNativeData.filePtr );
		return trunc_numeric_cast<file_size_t>( writtenBytesNum );
	}

	file_offset_t PosixFile::_nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
	{
		auto fileSeekPos = platform::_posixTranslateFilePointerRefPos( pRefPos );
		auto seekResult = ::fseek( mNativeData.filePtr, static_cast<long>( pOffset ), fileSeekPos );

		if( seekResult != 0 )
		{
			auto errnoString = platform::posixQueryErrnoStringByCode( errno );
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, std::move( errnoString ) );
		}

		auto currentFilePointer = ::ftell( mNativeData.filePtr );
		return static_cast<file_offset_t>( currentFilePointer );
	}

	file_offset_t PosixFile::_nativeGetFilePointer() const
	{
		auto currentFilePointer = ::ftell( mNativeData.filePtr );
		return static_cast<file_offset_t>( currentFilePointer );
	}

	file_size_t PosixFile::_nativeGetSize() const
	{
		auto savedFilePointer = ::ftell( mNativeData.filePtr );
		::fseek( mNativeData.filePtr, 0u, SEEK_END );

		auto fileSize = ::ftell( mNativeData.filePtr );
		::fseek( mNativeData.filePtr, savedFilePointer, SEEK_SET );

		return static_cast<file_size_t>( fileSize );
	}

	file_size_t PosixFile::_nativeGetRemainingBytes() const
	{
		auto currentFilePointer = ::ftell( mNativeData.filePtr );
		::fseek( mNativeData.filePtr, 0u, SEEK_END );

		auto fileSize = ::ftell( mNativeData.filePtr );
		::fseek( mNativeData.filePtr, currentFilePointer, SEEK_SET );

		return static_cast<file_size_t>( fileSize - currentFilePointer );
	}

	bool PosixFile::_nativeCheckEOF() const
	{
		return ::feof( mNativeData.filePtr ) != 0;
	}

	bool PosixFile::_nativeIsGood() const
	{
		return !::feof( mNativeData.filePtr ) && !::ferror( mNativeData.filePtr );
	}


	namespace platform
	{

		FILE * _posixOpenFileGeneric( const char * pFilePath, const char * pOpenMode )
		{
			FILE * filePtr = fopen( pFilePath, pOpenMode );

			if( !filePtr )
			{
				auto errnoString = platform::posixQueryErrnoStringByCode( errno );
				ts3ThrowDesc( E_EXC_SYSTEM_FILE_OPEN_ERROR, std::move( errnoString ) );
			}

			return filePtr;
		}

		const char * _posixTranslateFileOpenMode( EFileOpenMode pOpenMode )
		{
			switch( pOpenMode )
			{
				case EFileOpenMode::ReadOnly: return "r";
				case EFileOpenMode::ReadWrite: return "r+";
				case EFileOpenMode::WriteAppend: return "a";
				case EFileOpenMode::WriteOverwrite: return "w+";
			}
			return "r";
		}

		bool _posixIsFile( const char * pFilePath )
		{
			struct stat statInfo;
			auto statResult = ::stat( pFilePath, &statInfo );
			return ( statResult == 0 ) && ( ( statInfo.st_mode & S_IFMT ) == S_IFREG ) && ( ( statInfo.st_mode & S_IFMT ) == S_IFLNK );
		}

		int _posixTranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos )
		{
			int fileSeekPos = 0;

			switch( pFileRefPos )
			{
				case EFilePointerRefPos::FileBeg:
				{
					fileSeekPos = SEEK_SET;
					break;
				}
				case EFilePointerRefPos::FileEnd:
				{
					fileSeekPos = SEEK_END;
					break;
				}
				case EFilePointerRefPos::PtrCurrent:
				{
					fileSeekPos = SEEK_CUR;
					break;
				}
			}

			return fileSeekPos;
		}

		std::string _posixGenerateTempFileName()
		{
			return tmpnam( nullptr );
		}

	}

}
