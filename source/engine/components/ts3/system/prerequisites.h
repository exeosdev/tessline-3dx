
#ifndef __TS3_SYSTEM_PREREQUISITES_H__
#define __TS3_SYSTEM_PREREQUISITES_H__

#include <ts3/core/exception.h>
#include <ts3/stdext/bitmask.h>
#include <ts3/stdext/utilities.h>
#include <ts3/stdext/version.h>

#include <memory>
#include <string>
#include <utility>

#if( TS3_BUILD_STATIC )
#  define TS3_SYSTEM_API
#  define TS3_SYSTEM_CLASS
#  define TS3_SYSTEM_OBJ    extern
#else
#  if( TS3_SYSTEM_BUILD )
#    define TS3_SYSTEM_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_SYSTEM_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_SYSTEM_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3_SYSTEM_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_SYSTEM_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3_SYSTEM_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#define TS3_SYSTEM_API_NODISCARD TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD

namespace ts3::system
{

    ts3EnableExceptionSupport();

	template <typename TpObject>
	using Handle = ::ts3::SharedHandle<TpObject>;

	/// @brief Declares the handle type for a given system class.
	/// The handle's type name is created by appending 'Handle' suffix to a given class name.
	/// For example: ts3SysDeclareHandle( MyType ) will produce a declaration for 'MyTypeHandle'.
    #define ts3SysDeclareHandle( pType ) \
        using pType##Handle = Handle<class pType>

    // These two types need to be visible everywhere.
    ts3SysDeclareHandle( SysContext );
    ts3SysDeclareHandle( SysObject );

    enum : exception_category_value_t
    {
        E_EXCEPTION_CATEGORY_SYSTEM_CORE             = ecDeclareExceptionCategory( ExceptionBaseType::System, 1 ),
        E_EXCEPTION_CATEGORY_SYSTEM_DISPLAY          = ecDeclareExceptionCategory( ExceptionBaseType::System, 2 ),
        E_EXCEPTION_CATEGORY_SYSTEM_EVENT            = ecDeclareExceptionCategory( ExceptionBaseType::System, 3 ),
        E_EXCEPTION_CATEGORY_SYSTEM_FILE             = ecDeclareExceptionCategory( ExceptionBaseType::System, 4 ),
        E_EXCEPTION_CATEGORY_SYSTEM_OPENGL           = ecDeclareExceptionCategory( ExceptionBaseType::System, 5 ),
        E_EXCEPTION_CATEGORY_SYSTEM_WINDOW           = ecDeclareExceptionCategory( ExceptionBaseType::System, 9 ),
    };

    template <typename TpNativeData>
    class NativeObject
    {
    public:
        TpNativeData mNativeData;

    public:
        NativeObject() = default;
        virtual ~NativeObject() = default;
    };

    template <>
    class NativeObject<void>
    {
    public:
        NativeObject() = default;
        virtual ~NativeObject() = default;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PREREQUISITES_H__
