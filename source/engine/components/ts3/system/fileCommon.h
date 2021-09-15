
#ifndef __TS3_SYSTEM_FILE_COMMON_H__
#define __TS3_SYSTEM_FILE_COMMON_H__

#include "prerequisites.h"
#include <ts3/core/exception.h>

namespace ts3::system
{

    class File;
    class FileManager;

    using file_char_t = char;
    using file_str_t = std::basic_string<file_char_t>;
    using file_offset_t = native_int;
    using file_size_t = native_uint;

    using FileHandle = std::shared_ptr<File>;

    enum : exception_code_value_t
    {
        E_EXC_SYSTEM_FILE_OPEN_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_FILE, 1 )
    };

    enum class EFilePointerRefPos : enum_default_value_t
    {
        CurrentPos,
        FileBeg,
        FileEnd,
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

}

#endif //__TS3_SYSTEM_FILE_COMMON_H__
