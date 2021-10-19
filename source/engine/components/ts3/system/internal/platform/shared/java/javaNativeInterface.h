
#ifndef __TS3_SYSTEM_JAVA_NATIVE_INTERFACE_H__
#define __TS3_SYSTEM_JAVA_NATIVE_INTERFACE_H__

#include "javaCommon.h"

namespace ts3::system
{

    namespace platform
    {

        enum EAndroidScreenOrientation : int
        {
            E_ANDROID_SCREEN_ORIENTATION_LANDSCAPE         = 0,
            E_ANDROID_SCREEN_ORIENTATION_LOCKED            = 0x0000000E,
            E_ANDROID_SCREEN_ORIENTATION_PORTRAIT          = 0x00000001,
            E_ANDROID_SCREEN_ORIENTATION_REVERSE_LANDSCAPE = 0x00000008,
            E_ANDROID_SCREEN_ORIENTATION_REVERSE_PORTRAIT  = 0x00000009,
            E_ANDROID_SCREEN_ORIENTATION_UNSPECIFIED       = static_cast<int>( 0xFFFFFFFF ),
        };

    }

    class JavaNativeInterface final
    {
    public:
        JavaVMInstance * const mJVMInstance;
        platform::JNIThreadID const mJNIThreadID;
        platform::JNIEnvPtr const mJNIEnv;

    public:
        JavaNativeInterface( JavaVMInstance & pJVMInstance,
                             platform::JNIThreadID pJNIThreadID,
                             platform::JNIEnvPtr pJNIEnvPtr );

        ~JavaNativeInterface() noexcept;

        void nativeActivitySetRequestedOrientation( jobject pNativeActivity,
                                                    platform::EAndroidScreenOrientation pOrientation );
    };

}

#endif // __TS3_SYSTEM_JAVA_NATIVE_INTERFACE_H__
