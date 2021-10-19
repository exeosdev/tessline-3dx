
#include <ts3/system/fileManagerNative.h>

namespace ts3::system
{
    
    PosixFileManager::PosixFileManager( SysContextHandle pSysContext )
    : FileManager( std::move( pSysContext ) )
    {}

    PosixFileManager::~PosixFileManager() noexcept = default;

    FileHandle PosixFileManager::_nativeOpenFile( std::string pFilePath, EFileOpenMode pOpenMode )
    {
        return nullptr;
    }

    FileHandle PosixFileManager::_nativeCreateFile( std::string pFilePath )
    {
        return nullptr;
    }

    FileHandle PosixFileManager::_nativeCreateTemporaryFile()
    {
        return nullptr;
    }

    FileNameList PosixFileManager::_nativeEnumDirectoryFileNameList( const std::string & pDirectory )
    {
        return {};
    }

    std::string PosixFileManager::_nativeGenerateTemporaryFileName()
    {
        return "";
    }

    bool PosixFileManager::_nativeCheckDirectoryExists( const std::string & pDirPath )
    {
        return false;
    }

    bool PosixFileManager::_nativeCheckFileExists( const std::string & pFilePath )
    {
        return false;
    }


    PosixFile::PosixFile( FileManagerHandle pFileManager )
    : File( std::move( pFileManager ) )
    {}

    PosixFile::~PosixFile() noexcept
    {
        _releasePosixFileHandle();
    }

    void PosixFile::setInternalFilePtr( FILE * pFilePtr )
    {
    }

    void PosixFile::_releasePosixFileHandle()
    {
    }

    file_size_t PosixFile::_nativeReadData( void * pBuffer, file_size_t pBufferSize, file_size_t pReadSize )
    {
        return 0u;
    }

    file_offset_t PosixFile::_nativeSetFilePointer( file_offset_t pOffset, EFilePointerRefPos pRefPos )
    {
        return 0u;
    }

    file_size_t PosixFile::_nativeGetSize() const
    {
        return 0u;
    }

}
