
#ifndef __TS3_SYSTEM_JAVA_COMMON_H__
#define __TS3_SYSTEM_JAVA_COMMON_H__

#include <ts3/system/prerequisites.h>
#include <jni.h>
#include <thread>

namespace ts3::system
{

    class JavaNativeInterface;
    class JavaVMInstance;

    namespace platform
    {

        using JNIEnvPtr = JNIEnv *;
        using JNIThreadID = std::thread::id;

    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_JAVA_COMMON_H__
