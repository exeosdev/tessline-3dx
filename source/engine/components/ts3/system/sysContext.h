
#ifndef __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__

#include "prerequisites.h"

namespace ts3::system
{

    struct SysContextNativeData;

    ts3SysDeclareHandle( SysContext );

	/// @brief
	class SysContext
    {
    public:
        struct ContextPrivateData;
        std::unique_ptr<ContextPrivateData> const mInternal;
        const SysContextNativeData * const mNativeData = nullptr;

    public:
        SysContext();
        ~SysContext() noexcept;

        static std::string queryCurrentProcessExecutableDirectory();
        static std::string queryCurrentProcessExecutableFilePath();
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_H__
