
#ifndef __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__

#include "prerequisites.h"

namespace ts3
{
namespace system
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
        SysContextNativeProxy * const mNativeProxy = nullptr;
        SysContextNativeData * const mNativeDataPtr = nullptr;

    public:
        explicit SysContext( std::unique_ptr<SysContextNativeProxy> pNativeProxy );
        virtual ~SysContext();

    private:
        std::unique_ptr<SysContextNativeProxy> _nativeProxy;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
