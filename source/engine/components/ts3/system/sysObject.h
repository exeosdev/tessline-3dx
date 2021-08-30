
#ifndef __TS3_SYSTEM_SYS_OBJECT_H__
#define __TS3_SYSTEM_SYS_OBJECT_H__

#include "prerequisites.h"

namespace ts3::system
{

    /// @brief Base class for all system classes
    ///
    /// This class is used for easy injection of useful stuff (debug logs, instance counting, lifetime control)
    /// into all classes created by the System Library. It also provides (and, thus, enforces!) SysContext
    /// access, so it doesn't have to be declared multiple times (for every separate class in the library).
    class SysObject : public DynamicInterface
    {
    public:
        // Public handle to the SysContext. This is always non-null and specified at construction time.
        SysContextHandle const mSysContext;

        explicit SysObject( SysContextHandle pSysContext )
        : mSysContext( std::move( pSysContext ) )
        {}

        virtual ~SysObject() = default;
    };

    template <typename TpObject, typename... TpArgs>
    inline Handle<TpObject> createSysObject( TpArgs && ...pArgs )
    {
        return createDynamicInterfaceObject<TpObject>( std::forward<TpArgs>( pArgs )... );
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_SYS_OBJECT_H__
