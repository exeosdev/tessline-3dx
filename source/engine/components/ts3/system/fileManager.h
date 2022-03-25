
#ifndef __TS3_SYSTEM_FILE_MANAGER_H__
#define __TS3_SYSTEM_FILE_MANAGER_H__

#include "fileCommon.h"
#include "sysObject.h"
#include <ts3/stdext/byteArray.h>

namespace std
{

	template <typename _T, size_t _N>
	class array;

	template <typename _T, typename _Alloc>
	class vector;

}

namespace ts3::system
{

	class FileManager : public SysObject
	{
	public:
		explicit FileManager( SysContextHandle pSysContext );
		virtual ~FileManager() noexcept;

		FileHandle openFile( std::string pFilePath, EFileOpenMode pOpenMode );

		FileHandle createFile( std::string pFilePath );

		FileHandle createTemporaryFile();

		FileList openDirectoryFiles( const std::string & pDirectory );

		FileNameList enumDirectoryFiles( const std::string & pDirectory );

		std::string generateTemporaryFileName();

		bool checkDirectoryExists( const std::string & pDirPath );

		bool checkFileExists( const std::string & pFilePath );

	private:
		virtual FileHandle _nativeOpenFile( std::string pFilePath, EFileOpenMode pOpenMode ) = 0;
		virtual FileHandle _nativeCreateFile( std::string pFilePath ) = 0;
		virtual FileHandle _nativeCreateTemporaryFile() = 0;
		virtual FileNameList _nativeEnumDirectoryFileNameList( const std::string & pDirectory ) = 0;
		virtual std::string _nativeGenerateTemporaryFileName() = 0;
		virtual bool _nativeCheckDirectoryExists( const std::string & pDirPath ) = 0;
		virtual bool _nativeCheckFileExists( const std::string & pFilePath ) = 0;
	};

	class File : public SysObject
	{
	public:
		FileManagerHandle const mFileManager;

	public:
		explicit File( FileManagerHandle pFileManager );
		virtual ~File() noexcept;

		file_size_t read( void * pTargetBuffer, file_size_t pTargetBufferSize, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t read( const ReadWriteMemoryView & pTarget, file_size_t pReadSize = CX_FILE_SIZE_MAX );

		file_size_t readAuto( DynamicMemoryBuffer & pTarget, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t readAuto( DynamicByteArray & pTarget, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t readAuto( std::vector<byte> & pTarget, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t readAuto( std::string & pTarget, file_size_t pReadSize = CX_FILE_SIZE_MAX );

		file_size_t write( const void * pData, file_size_t pDataSize, file_size_t pWriteSize = CX_FILE_SIZE_MAX );
		file_size_t write( const ReadOnlyMemoryView & pSource, file_size_t pWriteSize = CX_FILE_SIZE_MAX );
		
		file_size_t write( const MemoryBuffer & pSource, file_size_t pWriteSize = CX_FILE_SIZE_MAX );
		file_size_t write( const ByteArray & pSource, file_size_t pWriteSize = CX_FILE_SIZE_MAX );
		file_size_t write( const std::vector<byte> & pSource, file_size_t pWriteSize = CX_FILE_SIZE_MAX );
		file_size_t write( const std::string & pSource, file_size_t pWriteSize = CX_FILE_SIZE_MAX );

		file_offset_t moveFilePointer( file_offset_t pOffset );

		file_offset_t setFilePointer( file_offset_t pPosition, EFilePointerRefPos pRefPos = EFilePointerRefPos::FileBeg );

		file_offset_t getFilePointer() const;

		file_size_t getSize() const;

		file_size_t getRemainingBytes() const;

		bool checkEOF() const;
		bool isGood() const;

	private:
		template <typename TpResizableBuffer>
		file_size_t _readAuto( TpResizableBuffer & pTarget, file_size_t pReadSize )
		{
			if( pReadSize == 0 )
			{
				return 0;
			}

			const auto remainingFileDataSize = _nativeGetRemainingBytes();
			const auto readSize = getMinOf( pReadSize, remainingFileDataSize );

			pTarget.resize( readSize );

			return _nativeReadData( pTarget.data(), readSize );
		}

		template <typename TpBuffer>
		file_size_t _write( const TpBuffer & pSource, file_size_t pWriteSize )
		{
			if( pWriteSize == 0 )
			{
				return 0;
			}

			const auto writeSize = getMinOf( pSource.size(), pWriteSize );

			return _nativeWriteData( pSource.data(), writeSize );
		}

		virtual file_size_t _nativeReadData( void * pTargetBuffer, file_size_t pReadSize ) = 0;
		virtual file_size_t _nativeWriteData( const void * pData, file_size_t pWriteSize ) = 0;
		virtual file_offset_t _nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos ) = 0;
		virtual file_offset_t _nativeGetFilePointer() const = 0;
		virtual file_size_t _nativeGetSize() const = 0;
		virtual file_size_t _nativeGetRemainingBytes() const = 0;
		virtual bool _nativeCheckEOF() const = 0;
		virtual bool _nativeIsGood() const = 0;
	};

}

#endif //__TS3_SYSTEM_FILE_MANAGER_H__
