
#ifndef __TS3_SYSTEM_SYS_CONTEXT_H__
#define __TS3_SYSTEM_SYS_CONTEXT_H__

#include "prerequisites.h"

namespace ts3
{
namespace system
{

    ts3DeclareHandle( Context );

	enum EContextCreateFlags : uint32
	{
		E_CONTEXT_CREATE_FLAG_INIT_DEFAULT = 0
	};

	struct ContextCreateInfo
	{
		Bitmask<EContextCreateFlags> flags = E_CONTEXT_CREATE_FLAG_INIT_DEFAULT;
	};

	/// @brief
	ContextHandle sysCreateContext( const ContextCreateInfo & pCreateInfo );

	/// @brief
	void sysDestroyContext( ContextHandle pContext );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_SYS_CONTEXT_H__
