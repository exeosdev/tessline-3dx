
#ifndef __TS3_SYSTEM_SYS_CONTEXT_H__
#define __TS3_SYSTEM_SYS_CONTEXT_H__

#include "prerequisites.h"

namespace ts3
{

    ts3DeclareSysHandle( SysContext );

	enum ESysContextCreateFlags : uint32
	{
		E_SYS_CONTEXT_CREATE_FLAG_INIT_DEFAULT = 0
	};

	struct SysContextCreateInfo
	{
		Bitmask<ESysContextCreateFlags> flags = E_SYS_CONTEXT_CREATE_FLAG_INIT_DEFAULT;
	};

	/// @brief
	SysContextHandle sysCreateContext( const SysContextCreateInfo & pCreateInfo );

	/// @brief
	void sysDestroyContext( SysContextHandle pContext );

}

#endif // __TS3_SYSTEM_SYS_CONTEXT_H__
6