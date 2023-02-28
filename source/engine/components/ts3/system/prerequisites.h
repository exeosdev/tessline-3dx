
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
#  define TS3_SYSTEM_OBJ extern __cdecl
#else
#  if( TS3_SYSTEM_BUILD )
#	define TS3_SYSTEM_API   TS3_PCL_ATTR_DLL_EXPORT
#	define TS3_SYSTEM_CLASS TS3_PCL_ATTR_DLL_EXPORT
#	define TS3_SYSTEM_OBJ   TS3_PCL_ATTR_DLL_EXPORT
#  else
#	define TS3_SYSTEM_API   TS3_PCL_ATTR_DLL_IMPORT
#	define TS3_SYSTEM_CLASS TS3_PCL_ATTR_DLL_IMPORT
#	define TS3_SYSTEM_OBJ   TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#define TS3_SYSTEM_API_NODISCARD TS3_SYSTEM_API TS3_PCL_ATTR_FUNC_NO_DISCARD

namespace ts3::system
{

	ts3EnableCustomExceptionSupport();
	ts3EnableEnumTypeInfoSupport();

	template <typename TObject>
	using SysHandle = ::ts3::SharedHandle<TObject>;

	/// @brief Declares the handle type for a given system class.
	/// The handle's type name is created by appending 'Handle' suffix to a given class name.
	/// For example: ts3SysDeclareHandle( MyType ) will produce a declaration for 'MyTypeHandle'.
	#define ts3SysDeclareHandle( pType ) \
		class pType; using pType##Handle = SysHandle<::ts3::system::pType>

	// These two types need to be visible everywhere.
	ts3SysDeclareHandle( SysContext );
	ts3SysDeclareHandle( SysObject );

	enum : exception_category_value_t
	{
		E_EXCEPTION_CATEGORY_SYSTEM_CORE    = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x01 ),
		E_EXCEPTION_CATEGORY_SYSTEM_DISPLAY = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x02 ),
		E_EXCEPTION_CATEGORY_SYSTEM_EVENT   = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x03 ),
		E_EXCEPTION_CATEGORY_SYSTEM_FILE    = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x04 ),
		E_EXCEPTION_CATEGORY_SYSTEM_METAL   = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x05 ),
		E_EXCEPTION_CATEGORY_SYSTEM_OPENGL  = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x07 ),
		E_EXCEPTION_CATEGORY_SYSTEM_WINDOW  = ecDeclareExceptionCategory( ExceptionBaseType::System, 0x09 ),
	};

	enum : exception_code_value_t
	{
		E_EXC_SYSTEM_INTERFACE_NOT_SUPPORTED = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_SYSTEM_CORE, 0x04 ),
	};

	/// @brief Helper proxy-like base class for platform-specific types.
	/// @tparam TBaseType Base class to derive from.
	/// @tparam TNativeData Native data type with platform-specific state.
	///
	/// Public classes in the System component (like File, Window, GLDriver etc.) are implemented at the platform
	/// level (Win32File, Win32Window, Win32GLDriver). These classes obviously carry some platform-specific data.
	/// This class serves as a helper which adds public mNativeData member of a specified type and inherits from
	/// a given base class. It enables much compact definition of platform-specific types - especially those, that
	/// only require native data (like Adapter/Output/VideoMode from the display system) - they can be defined as a
	/// simple typedef.
	template <typename TBaseType, typename TNativeData>
	class NativeObject : public TBaseType
	{
	public:
		TNativeData mNativeData;

	public:
		template <typename... TBaseArgs>
		explicit NativeObject( TBaseArgs && ...pBaseArgs )
		: TBaseType( std::forward<TBaseArgs>( pBaseArgs )... )
		{}

		virtual ~NativeObject() = default;
	};

	template <typename TBaseType>
	class NativeObject<TBaseType, void> : public TBaseType
	{
	public:
		template <typename... TBaseArgs>
		explicit NativeObject( TBaseArgs && ...pBaseArgs )
		: TBaseType( std::forward<TBaseArgs>( pBaseArgs )... )
		{}

		virtual ~NativeObject() = default;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PREREQUISITES_H__
