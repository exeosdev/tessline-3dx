
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

		file_size_t readData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t readData( const ReadWriteMemoryView & pBuffer, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t readData( DynamicMemoryBuffer & pBuffer, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t readData( DynamicByteArray & pBuffer, file_size_t pReadSize = CX_FILE_SIZE_MAX );
		file_size_t readData( std::vector<byte> & pBuffer, file_size_t pReadSize = CX_FILE_SIZE_MAX );

		template <size_t tpSize>
		file_size_t readData( std::array<byte, tpSize> & pBuffer, file_size_t pReadSize = CX_FILE_SIZE_MAX )
		{
			return readData( pBuffer.data(), pBuffer.size(), pReadSize );
		}

		file_size_t writeData( const void * pBuffer, file_size_t pBufferSize, file_size_t pWriteSize = CX_FILE_SIZE_MAX );

		file_offset_t setFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos = EFilePointerRefPos::FileBeg );

		file_offset_t getFilePointer() const;

		file_size_t getSize() const;

		bool checkEOF() const;
		bool isGood() const;

	private:
		virtual file_size_t _nativeReadData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize ) = 0;
		virtual file_size_t _nativeWriteData( const void * pBuffer, file_size_t pBufferSize, file_size_t pWriteSize ) = 0;
		virtual file_offset_t _nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos ) = 0;
		virtual file_offset_t _nativeGetFilePointer() const = 0;
		virtual file_size_t _nativeGetSize() const = 0;
		virtual bool _nativeCheckEOF() const = 0;
		virtual bool _nativeIsGood() const = 0;
	};

}

#endif //__TS3_SYSTEM_FILE_MANAGER_H__
