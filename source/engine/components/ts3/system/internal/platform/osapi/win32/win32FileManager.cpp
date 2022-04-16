
#include <ts3/system/fileManagerNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

	namespace platform
	{

		HANDLE _win32OpenFileGeneric( const char * pFilePath, DWORD pFileAccess, DWORD pOpenMode, DWORD pFileFlags );

		void _win32CloseFile( HANDLE pFileHandle );

		DWORD _win32TranslateFileOpenModeToWin32Access( EFileOpenMode pOpenMode );

		DWORD _win32TranslateFileOpenModeToWin32CreationDisposition( EFileOpenMode pOpenMode );

		DWORD _win32TranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos );

		std::string _win32GenerateTempFileName();

	}
	
	
	Win32FileManager::Win32FileManager( SysContextHandle pSysContext )
	: Win32NativeObject( std::move( pSysContext ) )
	{}

	Win32FileManager::~Win32FileManager() noexcept = default;

	FileHandle Win32FileManager::_nativeOpenFile( std::string pFilePath, EFileOpenMode pOpenMode )
	{
		const auto fileAccess = platform::_win32TranslateFileOpenModeToWin32Access( pOpenMode );
		const auto creationDisposition = platform::_win32TranslateFileOpenModeToWin32CreationDisposition( pOpenMode );

		auto fileHandle = platform::_win32OpenFileGeneric( pFilePath.c_str(), fileAccess, creationDisposition, FILE_ATTRIBUTE_NORMAL );
		auto fileObject = createSysObject<Win32File>( getHandle<Win32FileManager>() );
		fileObject->setInternalWin32FileHandle( fileHandle );

		return fileObject;
	}

	FileHandle Win32FileManager::_nativeCreateFile( std::string pFilePath )
	{
		const auto fileAccess = GENERIC_READ | GENERIC_WRITE;
		const auto creationDisposition = CREATE_ALWAYS;

		auto fileHandle = platform::_win32OpenFileGeneric( pFilePath.c_str(), fileAccess, creationDisposition, FILE_ATTRIBUTE_NORMAL );
		auto fileObject = createSysObject<Win32File>( getHandle<Win32FileManager>() );
		fileObject->setInternalWin32FileHandle( fileHandle );

		return fileObject;
	}

	FileHandle Win32FileManager::_nativeCreateTemporaryFile()
	{
		const auto fileAccess = GENERIC_READ | GENERIC_WRITE;
		const auto creationDisposition = CREATE_ALWAYS;

		auto tempFilePath = platform::_win32GenerateTempFileName();

		auto fileHandle = platform::_win32OpenFileGeneric( tempFilePath.c_str(), fileAccess, creationDisposition, FILE_ATTRIBUTE_TEMPORARY );
		auto fileObject = createSysObject<Win32File>( getHandle<Win32FileManager>() );
		fileObject->setInternalWin32FileHandle( fileHandle );

		return fileObject;
	}

	FileNameList Win32FileManager::_nativeEnumDirectoryFileNameList( const std::string & pDirectory )
	{
		FileNameList resultList;

		auto targetDirectory = pDirectory;
		targetDirectory.append( 1, TS3_PCL_ENV_DEFAULT_PATH_DELIMITER );
		targetDirectory.append( 1, '*' );

		WIN32_FIND_DATAA win32FindFileData;
		auto win32FindFileHandle = ::FindFirstFileA( targetDirectory.c_str(), &win32FindFileData );

		if( win32FindFileHandle == INVALID_HANDLE_VALUE )
		{
			auto lastError = ::GetLastError();
			auto lastErrorMessage = platform::mseQuerySystemErrorMessage( lastError );
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, std::move( lastErrorMessage ) );
		}

		while( win32FindFileHandle )
		{
			auto fileName = std::string( win32FindFileData.cFileName );
			auto fileAttributes = makeBitmask( win32FindFileData.dwFileAttributes );

			if( !fileAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY ) && ( fileName != "." ) && ( fileName != ".." ) )
			{
				resultList.push_back( std::string( win32FindFileData.cFileName ) );
			}

			auto findNextResult = ::FindNextFileA( win32FindFileHandle, &win32FindFileData );

			if( !findNextResult )
			{
				auto lastError = ::GetLastError();
				if( lastError == ERROR_NO_MORE_FILES )
				{
					::FindClose( win32FindFileHandle );
					win32FindFileHandle = nullptr;
				}
				else
				{
					auto lastErrorMessage = platform::mseQuerySystemErrorMessage( lastError );
					ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, std::move( lastErrorMessage ) );
				}
			}
		}

		return resultList;
	}

	std::string Win32FileManager::_nativeGenerateTemporaryFileName()
	{
		return platform::_win32GenerateTempFileName();
	}

	bool Win32FileManager::_nativeCheckDirectoryExists( const std::string & pDirPath )
	{
		Bitmask<DWORD> targetAttributes = ::GetFileAttributesA( pDirPath.c_str() );
		return ( targetAttributes != INVALID_FILE_ATTRIBUTES ) && targetAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY );
	}

	bool Win32FileManager::_nativeCheckFileExists( const std::string & pFilePath )
	{
		Bitmask<DWORD> targetAttributes = ::GetFileAttributesA( pFilePath.c_str() );
		return ( targetAttributes != INVALID_FILE_ATTRIBUTES ) && !targetAttributes.isSet( FILE_ATTRIBUTE_DIRECTORY );
	}
	
	
	Win32File::Win32File( FileManagerHandle pFileManager )
	: Win32NativeObject( std::move( pFileManager ) )
	{}

	Win32File::~Win32File() noexcept
	{
		_releaseWin32FileHandle();
	}

	void Win32File::setInternalWin32FileHandle( HANDLE pFileHandle )
	{
		ts3DebugAssert( !mNativeData.fileHandle );
		mNativeData.fileHandle = pFileHandle;
	}

	void Win32File::_releaseWin32FileHandle()
	{
		if( mNativeData.fileHandle )
		{
			platform::_win32CloseFile( mNativeData.fileHandle );
			mNativeData.fileHandle = nullptr;
		}
	}

	file_size_t Win32File::_nativeReadData( void * pTargetBuffer, file_size_t pReadSize )
	{
		DWORD readBytesNum = 0u;
		auto readResult = ::ReadFile( mNativeData.fileHandle, pTargetBuffer, pReadSize, &readBytesNum, nullptr );

		if( readResult && ( readBytesNum == 0 ) )
		{
			mNativeData.flags.set( platform::E_WIN32_FILE_FLAG_EOF_BIT );
		}

		if( !readResult )
		{
			auto errorCode = ::GetLastError();
			if( errorCode == ERROR_HANDLE_EOF )
			{
				mNativeData.flags.set( platform::E_WIN32_FILE_FLAG_EOF_BIT );
			}
			else
			{
				auto errorMessage = platform::mseQuerySystemErrorMessage( errorCode );
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
			}
		}

		return trunc_numeric_cast<file_size_t>( readBytesNum );
	}

	file_size_t Win32File::_nativeWriteData( const void * pData, file_size_t pWriteSize )
	{
		DWORD writtenBytesNum = 0u;
		auto writeResult = ::WriteFile(  mNativeData.fileHandle, pData, pWriteSize, &writtenBytesNum, nullptr );

		if( !writeResult )
		{
			auto errorCode = ::GetLastError();
			auto errorMessage = platform::mseQuerySystemErrorMessage( errorCode );
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
		}

		return trunc_numeric_cast<file_size_t>( writtenBytesNum );
	}

	file_offset_t Win32File::_nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
	{
		LARGE_INTEGER u64FileOffset;
		u64FileOffset.QuadPart = static_cast<decltype( u64FileOffset.QuadPart )>( pOffset );

		auto win32FPMoveMode = platform::_win32TranslateFilePointerRefPos( pRefPos );

		u64FileOffset.LowPart = ::SetFilePointer( mNativeData.fileHandle,
		                                          u64FileOffset.LowPart,
		                                          &( u64FileOffset.HighPart ),
		                                          win32FPMoveMode );

		if( u64FileOffset.LowPart == INVALID_SET_FILE_POINTER )
		{
			auto lastError = ::GetLastError();
			if( lastError != NO_ERROR )
			{
				auto errorMessage = platform::mseQuerySystemErrorMessage( lastError );
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, std::move( errorMessage ) );
			}
		}

		return trunc_numeric_cast<file_offset_t>( u64FileOffset.QuadPart );
	}

	file_offset_t Win32File::_nativeGetFilePointer() const
	{
		LARGE_INTEGER u64SetFileOffset;
		u64SetFileOffset.QuadPart = 0u;

		LARGE_INTEGER u64GetFilePosition;
		u64GetFilePosition.QuadPart = 0u;

		::SetFilePointerEx( mNativeData.fileHandle, u64SetFileOffset, &u64GetFilePosition, FILE_CURRENT );

		return trunc_numeric_cast<file_offset_t>( u64GetFilePosition.QuadPart );
	}

	file_size_t Win32File::_nativeGetSize() const
	{
		LARGE_INTEGER u64FileSize;
		u64FileSize.QuadPart = 0L;

		::GetFileSizeEx( mNativeData.fileHandle, &u64FileSize );

		return trunc_numeric_cast<file_size_t>( u64FileSize.QuadPart );
	}

	file_size_t Win32File::_nativeGetRemainingBytes() const
	{
		LARGE_INTEGER u64SetFileOffset;
		u64SetFileOffset.QuadPart = 0u;

		LARGE_INTEGER u64GetFilePosition;
		u64GetFilePosition.QuadPart = 0u;

		::SetFilePointerEx( mNativeData.fileHandle, u64SetFileOffset, &u64GetFilePosition, FILE_CURRENT );
		const auto previousFilePointer = u64GetFilePosition.QuadPart;

		::SetFilePointerEx( mNativeData.fileHandle, u64SetFileOffset, &u64GetFilePosition, FILE_END );
		const auto endFilePosition = u64GetFilePosition.QuadPart;

		u64SetFileOffset.QuadPart = previousFilePointer;

		::SetFilePointerEx( mNativeData.fileHandle, u64SetFileOffset, nullptr, FILE_BEGIN );

		return trunc_numeric_cast<file_offset_t>( endFilePosition - previousFilePointer );
	}

	bool Win32File::_nativeCheckEOF() const
	{
		return mNativeData.flags.isSet( platform::E_WIN32_FILE_FLAG_EOF_BIT );
	}

	bool Win32File::_nativeIsGood() const
	{
		return mNativeData.flags == 0;
	}


	namespace platform
	{

		HANDLE _win32OpenFileGeneric( const char * pFilePath, DWORD pFileAccess, DWORD pOpenMode, DWORD pFileFlags )
		{
			auto fileHandle = ::CreateFileA( pFilePath, pFileAccess, 0u, nullptr, pOpenMode, pFileFlags, nullptr );

			if( fileHandle == INVALID_HANDLE_VALUE )
			{
				auto lastErrorCode = ::GetLastError();
				auto errorMessage = platform::mseQuerySystemErrorMessage( lastErrorCode );
				ts3ThrowDesc( E_EXC_SYSTEM_FILE_OPEN_ERROR, std::move( errorMessage ) );
			}

			return fileHandle;
		}

		void _win32CloseFile( HANDLE pFileHandle )
		{
			auto closeResult = ::CloseHandle( pFileHandle );

			if( closeResult == FALSE )
			{
				auto lastErrorCode = ::GetLastError();
				auto errorMessage = platform::mseQuerySystemErrorMessage( lastErrorCode );
				ts3DebugInterrupt();
			}
		}

		DWORD _win32TranslateFileOpenModeToWin32Access( EFileOpenMode pOpenMode )
		{
			switch( pOpenMode )
			{
			case EFileOpenMode::ReadOnly:
				return GENERIC_READ;

			case EFileOpenMode::ReadWrite:
				return GENERIC_READ | GENERIC_WRITE;

			case EFileOpenMode::WriteAppend:
				return GENERIC_WRITE;

			case EFileOpenMode::WriteOverwrite:
				return GENERIC_READ | GENERIC_WRITE;

			default:
				break;
			}

			return GENERIC_READ | GENERIC_WRITE;
		}

		DWORD _win32TranslateFileOpenModeToWin32CreationDisposition( EFileOpenMode pOpenMode )
		{
			switch( pOpenMode )
			{
			case EFileOpenMode::ReadOnly:
				return OPEN_EXISTING;

			case EFileOpenMode::ReadWrite:
				return OPEN_ALWAYS;

			case EFileOpenMode::WriteAppend:
				return OPEN_ALWAYS;

			case EFileOpenMode::WriteOverwrite:
				return CREATE_ALWAYS;

			default:
				break;
			}

			return OPEN_ALWAYS;
		}

		DWORD _win32TranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos )
		{
			DWORD win32FPMoveMode = 0;

			switch( pFileRefPos )
			{
				case EFilePointerRefPos::FileBeg:
				{
					win32FPMoveMode = FILE_BEGIN;
					break;
				}
				case EFilePointerRefPos::FileEnd:
				{
					win32FPMoveMode = FILE_END;
					break;
				}
				case EFilePointerRefPos::PtrCurrent:
				{
					win32FPMoveMode = FILE_CURRENT;
					break;
				}
			}

			return win32FPMoveMode;
		}

		std::string _win32GenerateTempFileName()
		{
			file_char_t tempFileDir[MAX_PATH];
			::GetTempPathA( MAX_PATH, tempFileDir );

			file_char_t tempFileName[MAX_PATH];
			::GetTempFileNameA( tempFileDir, "TS3", 0, tempFileName );

			return std::string( tempFileName );
		}

	}

}
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
