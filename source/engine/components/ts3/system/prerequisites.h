
#ifndef __TS3_SYSTEM_PREREQUISITES_H__
#define __TS3_SYSTEM_PREREQUISITES_H__

#include <ts3/core/prerequisites.h>
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

	template <typename TpObject>
	using Handle = SharedHandle<TpObject>;

	/// @brief Declares the handle type for a given system class.
	/// The handle's type name is created by appending 'Handle' suffix to a given class name.
	/// For example: ts3SysDeclareHandle( MyType ) will produce a declaration for 'MyTypeHandle'.
    #define ts3SysDeclareHandle( pType ) \
        using pType##Handle = Handle<class pType>

    // These two types need to be visible everywhere.
    ts3SysDeclareHandle( SysContext );
    ts3SysDeclareHandle( SysObject );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PREREQUISITES_H__
