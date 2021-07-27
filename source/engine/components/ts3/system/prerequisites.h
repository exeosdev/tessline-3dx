
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

#define ts3SysDeclarePlatformTypeHandleImpl( pName, pPlatform ) \
    using pName##Handle = struct pPlatform##pName##NativeData *

namespace ts3
{

    class SysContext;
	class SystemInterface;

	template <typename TpObject>
	using SysHandle = SharedHandle<TpObject>;

#define ts3DeclareSysHandle( pType ) \
	class pType; using pType##Handle = SysHandle<pType>

	ts3DeclareSysHandle( SysBaseObject );
	ts3DeclareSysHandle( SysContext );

	/// @brief
	class SysBaseObject : public DynamicInterface
	{
	public:
		SysContextHandle const mSysContext;

		explicit SysBaseObject( SysContextHandle pSysContext )
		: mSysContext( std::move( pSysContext ) )
		{}

		virtual ~SysBaseObject() = default;
	};

	template <typename TpObject, typename... TpArgs>
	inline SysHandle<TpObject> sysCreateObject( TpArgs && ...pArgs )
	{
		return createDynamicInterfaceObject<TpObject>( std::forward<TpArgs>( pArgs )... );
	}
}

#endif // __TS3_SYSTEM_PREREQUISITES_H__
