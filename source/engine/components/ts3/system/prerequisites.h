
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
        class pType; using pType##Handle = Handle<::ts3::system::pType>

    // These two types need to be visible everywhere.
    ts3SysDeclareHandle( SysContext );
    ts3SysDeclareHandle( SysObject );

    enum : exception_category_value_t
    {
        E_EXCEPTION_CATEGORY_SYSTEM_CORE    = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x01 ),
        E_EXCEPTION_CATEGORY_SYSTEM_DISPLAY = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x02 ),
        E_EXCEPTION_CATEGORY_SYSTEM_EVENT   = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x03 ),
        E_EXCEPTION_CATEGORY_SYSTEM_FILE    = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x04 ),
        E_EXCEPTION_CATEGORY_SYSTEM_OPENGL  = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x05 ),
        E_EXCEPTION_CATEGORY_SYSTEM_WINDOW  = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x09 ),
    };

    template <typename TpBaseType, typename TpNativeData>
    class NativeObject : public TpBaseType
    {
    public:
        TpNativeData mNativeData;

    public:
        template <typename... TpBaseArgs>
        NativeObject( TpBaseArgs && ...pBaseArgs )
        : TpBaseType( std::forward<TpBaseArgs>( pBaseArgs )... )
        {}

        virtual ~NativeObject() = default;
    };

    template <typename TpBaseType>
    class NativeObject<TpBaseType, void>
    {
    public:
        template <typename... TpBaseArgs>
        NativeObject( TpBaseArgs && ...pBaseArgs )
        : TpBaseType( std::forward<TpBaseArgs>( pBaseArgs )... )
        {}

        virtual ~NativeObject() = default;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PREREQUISITES_H__
