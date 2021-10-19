
#ifndef __TS3_SYSTEM_FILE_COMMON_H__
#define __TS3_SYSTEM_FILE_COMMON_H__

#include "prerequisites.h"
#include <ts3/core/exception.h>

namespace ts3::system
{

    ts3SysDeclareHandle( FileManager );

    using FileHandle = std::shared_ptr<class File>;
    using FileList = std::vector<FileHandle>;
    using FileNameList = std::vector<std::string>;

    using file_char_t = char;
    using file_str_t = std::basic_string<file_char_t>;
    using file_offset_t = native_int;
    using file_size_t = native_uint;

    inline constexpr auto CX_FILE_SIZE_MAX = Limits<file_size_t>::maxValue;

    enum : exception_code_value_t
    {
        E_EXC_SYSTEM_FILE_OPEN_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_FILE, 1 )
    };

    enum class EFilePointerRefPos : enum_default_value_t
    {
        FileBeginning,
        FileEnd,
        PtrCurrent,
    };

    enum class EFileOpenMode : enum_default_value_t
    {
        // Open file for reading. If the specified file does not exist, an error is reported.
        // - File pointer is set to a BEGINNING.
        // - Existing content is PRESERVED.
        ReadOnly,

        // Open file for reading and writing. If the specified file does not exist, a new one is created.
        // - File pointer is set to a BEGINNING.
        // - Existing content is PRESERVED.
        ReadWrite,

        // Open file for writing. If the specified file does not exist, a new one is created.
        // - File pointer is set to an END.
        // - Existing content is PRESERVED.
        WriteAppend,

        // Open file for writing. If the specified file does not exist, a new one is created.
        // - File pointer is set to a BEGINNING.
        // - Existing content is ERASED.
        WriteOverwrite
    };

    class FileException : public SystemException
    {
    public:
        std::string mFilePath;

    public:
        explicit FileException( ExceptionInfo pInfo, std::string pFilePath = {} )
        : SystemException( std::move( pInfo ) )
        , mFilePath( std::move( pFilePath ) )
        {}
    };

    ts3SetExceptionCategoryType( E_EXCEPTION_CATEGORY_SYSTEM_FILE, FileException );

    enum EFileAPIFlags : uint32
    {
        E_FILE_API_FLAG_SPLIT_PATH_ASSUME_DIRECTORY_BIT     = 0x0001,

        E_FILE_API_FLAG_SPLIT_PATH_ASSUME_FILE_BIT          = 0x0002,

        E_FILE_API_FLAGS_SPLIT_PATH_DEFAULT = E_FILE_API_FLAG_SPLIT_PATH_ASSUME_DIRECTORY_BIT
    };

    class FileUtilityAPI
    {
    public:
        struct FilePathInfo
        {
            std::string directory;
            std::string fileName;
        };

    public:
        static std::string normalizePath( const std::string & pPath );

        static FilePathInfo splitFilePath( std::string pFilePath, Bitmask<EFileAPIFlags> pFlags = E_FILE_API_FLAGS_SPLIT_PATH_DEFAULT );

        static bool isFilenameWithExtension( const std::string & pFilename );
    };

}

#endif //__TS3_SYSTEM_FILE_COMMON_H__
