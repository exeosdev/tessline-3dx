
#ifndef __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__

#include "prerequisites.h"

namespace ts3::system
{

    ts3SysDeclareHandle( SysContext );

    ts3SysDeclareNativeTypes( SysContext );

    /// @brief
	enum ESysContextCreateFlags : uint32
	{
		E_CORE_SESSION_CONTEXT_CREATE_FLAG_INIT_DEFAULT = 0
	};

	/// @brief
	struct SysContextCreateInfo
	{
		Bitmask<ESysContextCreateFlags> flags = E_CORE_SESSION_CONTEXT_CREATE_FLAG_INIT_DEFAULT;
	};

	/// @brief
	class SysContext
    {
    public:
        struct ContextPrivateData;
        std::unique_ptr<ContextPrivateData> const mPrivateData;
        const SysContextNativeData * const mNativeData = nullptr;

    public:
        SysContext();
        ~SysContext();
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
