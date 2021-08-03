
#ifndef __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__

#include "prerequisites.h"

namespace ts3
{
namespace system
{

    ts3DeclareHandle( CoreSessionContext );

	enum ECoreSessionContextCreateFlags : uint32
	{
		E_CORE_SESSION_CONTEXT_CREATE_FLAG_INIT_DEFAULT = 0
	};

	struct CoreSessionContextCreateInfo
	{
		Bitmask<ECoreSessionContextCreateFlags> flags = E_CORE_SESSION_CONTEXT_CREATE_FLAG_INIT_DEFAULT;
	};

	/// @brief
	CoreSessionContextHandle createCoreSessionContext( const CoreSessionContextCreateInfo & pCreateInfo );

	/// @brief
	void destroyCoreSessionContext( CoreSessionContextHandle pContext );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
