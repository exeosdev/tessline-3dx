
#ifndef __TS3_SYSTEM_SYSTEM_CONTEXT_H__
#define __TS3_SYSTEM_SYSTEM_CONTEXT_H__

#include "prerequisites.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidSystemContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32SystemContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/sysX11temContext.h"
#endif

namespace ts3
{

	enum ESysContextCreateFlags : uint32
	{
		E_SYS_CONTEXT_CREATE_FLAG_INIT_DEFAULT = 0
	};

	struct SysContextCreateInfo : public SysContextNativeCreateInfo
	{
		Bitmask<ESysContextCreateFlags> flags = E_SYS_CONTEXT_CREATE_FLAG_INIT_DEFAULT;
	};

	/// @brief
	class SysContext
	{
		friend SysContextHandle sysCreateContext( const SysContextCreateInfo & pCreateInfo );

	public:
		SysContextNativeData mNativeData;

		SysContext() noexcept;
		~SysContext() noexcept;

	private:
		void _sysInitialize( const SysContextCreateInfo & pCreateInfo );
		void _sysRelease() noexcept;
	};

	SysContextHandle sysCreateContext( const SysContextCreateInfo & pCreateInfo );

}

#endif // __TS3_SYSTEM_SYSTEM_CONTEXT_H__
