
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

namespace ts3
{
namespace system
{

    class Context;
	class temInterface;

	template <typename TpObject>
	using Handle = SharedHandle<TpObject>;

#define ts3DeclareHandle( pType ) \
	using pType##Handle = struct pType *

	ts3DeclareHandle( BaseObject );
	ts3DeclareHandle( Context );

	/// @brief
	class BaseObject : public DynamicInterface
	{
	public:
		ContextHandle const mContext;

		explicit BaseObject( ContextHandle pContext )
		: mContext( std::move( pContext ) )
		{}

		virtual ~BaseObject() = default;
	};

	template <typename TpObject, typename... TpArgs>
	inline Handle<TpObject> sysCreateObject( TpArgs && ...pArgs )
	{
		return createDynamicInterfaceObject<TpObject>( std::forward<TpArgs>( pArgs )... );
	}

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PREREQUISITES_H__
