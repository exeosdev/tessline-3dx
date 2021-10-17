
#ifndef __TS3_SYSTEM_JAVA_VM_H__
#define __TS3_SYSTEM_JAVA_VM_H__

#include "javaCommon.h"
#include <memory>

namespace ts3::system
{

    namespace platform
    {

        enum EJNIThreadStateFlags : enum_default_value_t
        {
            E_JNI_THREAD_STATE_AUTO_RELEASE_BIT = 0x8000
        };

        struct JNIThreadState
        {
            JNIThreadID jniThreadID;

            std::atomic<uint32> jniRefCounter;

            JNIEnvPtr jniEnvPtr = nullptr;

            JavaNativeInterface * jniObject = nullptr;

            Bitmask<EJNIThreadStateFlags> flags = 0;
        };

        struct JavaNativeInterfaceDeleter
        {
            void operator()( JavaNativeInterface * pJNI ) const;
        };

    }

    using JavaNativeInterfacePtr = std::shared_ptr<JavaNativeInterface>;

    class JavaVMInstance final
    {
        friend struct platform::JavaNativeInterfaceDeleter;

    public:
        JavaVM * const mJavaVM;

    public:
        explicit JavaVMInstance( JavaVM * pJavaVM );
        ~JavaVMInstance() noexcept;

        JavaNativeInterfacePtr acquireJNI( platform::JNIThreadID pJNIThreadID );

        JavaNativeInterfacePtr acquireJNIForCurrentThread();

        void setCurrentThreadAutoReleaseState( bool pAutoRelease );

        void releaseCurrentThreadState();

    friendapi:
        void onJavaNativeInterfacePtrDestroy( JavaNativeInterface * pJNI );

    private:
        JavaNativeInterface * _acquireJNI( platform::JNIThreadID pJNIThreadID, bool pAutoCreateState );

        platform::JNIThreadState * _getJNIThreadState( platform::JNIThreadID pJNIThreadID, bool pAutoCreateState );

        void _onJNIThreadStateDestroyRequest( platform::JNIThreadID pJNIThreadID, bool pForceRelease );

    private:
        struct JavaVMInstancePrivateData;
        std::unique_ptr<JavaVMInstancePrivateData> _privateData;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_JAVA_VM_H__
