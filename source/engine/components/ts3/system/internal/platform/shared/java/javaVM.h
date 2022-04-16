
#ifndef __TS3_SYSTEM_JAVA_VM_H__
#define __TS3_SYSTEM_JAVA_VM_H__

#include "javaCommon.h"
#include <memory>

namespace ts3::system
{

	namespace platform
	{

		/// @brief Internal flags used for ThreadState.
		enum EJNIThreadStateFlags : enum_default_value_t
		{
			// If set, the thread's JNI state is released automatically when the internal
			// ref counter drops to 0. See JavaVMInstance::setCurrentThreadAutoReleaseState().
			E_JNI_THREAD_STATE_AUTO_RELEASE_BIT = 0x8000
		};

		/// @brief Thread-specific JNI-related data. Created for each thread attached to the VM.
		struct JNIThreadState
		{
			// ID of the thread this state belongs to.
			JNIThreadID jniThreadID;
			// Internal reference counter. Incremented for every new JNIPtr created.
			std::atomic<uint32> jniRefCounter;
			// Pointer to the JNIEnv struct.
			JNIEnvPtr jniEnvPtr = nullptr;
			// JavaNativeInterface object.
			JavaNativeInterface * jniObject = nullptr;
			// Internal flags.
			Bitmask<EJNIThreadStateFlags> flags = 0;
		};

		struct JavaNativeInterfaceDeleter
		{
			void operator()( JavaNativeInterface * pJNI ) const;
		};

	}

	/// @brief Represents an instance of the Java Virtual Machine. Associated with a JavaVM pointer.
	/// @details
	/// The core interface of the Java support layer. It is created for an existing JavaVM pointer,
	/// provides abstraction on top of it and adds a thread-safe JNI management API.
	class JavaVMInstance final
	{
		friend struct platform::JavaNativeInterfaceDeleter;

	public:
		// The JavaVM pointer associated with the instance.
		JavaVM * const mJavaVM;

	public:
		explicit JavaVMInstance( JavaVM * pJavaVM );
		~JavaVMInstance() noexcept;

		/// @brief Acquires an active reference to the JNI object of the specified thread.
		/// @details
		/// This function provides a way to obtain any thread's JNI object using its thread ID.
		/// If requested thread's state has not yet been created, this function will automatically
		/// do this *IF* (and only if) the current calling thread is the one whose JNI is being
		/// requested. In other words, a thread must first initialize its state before anyone else
		/// can retrieve it. This can be done with initializeCurrentThreadJNIState().
		JavaNativeInterfacePtr acquireJNI( platform::JNIThreadID pJNIThreadID );

		/// @brief Acquires an active reference to the JNI object of the current thread.
		/// @see JavaVMInstance::acquireJNI
		JavaNativeInterfacePtr acquireJNIForCurrentThread();

		/// @brief Initializes the JNI state for the current thread.
		void initializeCurrentThreadJNIState();

		/// @brief Sets the auto-release state for the current thread.
		/// @details
		/// By default, when the last instance of JavaNativeInterfacePtr is destroyed, it only causes
		/// the internal reference counter to be decremented - the JNI object itself is kept alive,
		/// so it can be again quickly retrieved. In this case, releaseCurrentThreadState() needs
		/// to be called before thread finishes execution. To make this an automated process, call
		/// this function with 'true', which will cause the JNI state to be release when ref counter
		/// drops to 0. A drawback is that the JNI state needs to re-created every time it's acquired.
		void setCurrentThreadAutoReleaseState( bool pAutoRelease );

		/// @brief Releases all JNI resources created for the current thread.
		void releaseCurrentThreadState();

	friendapi:
		/// @brief Used by JavaNativeInterfaceDeleter, called when JNI objects is to be destroyed.
		void onJavaNativeInterfacePtrDestroy( JavaNativeInterface * pJNI );

	private:
		// Returns the JNI object for a specified thread. If not present, auto-creates it if requested.
		JavaNativeInterface * _acquireJNI( platform::JNIThreadID pJNIThreadID, bool pAutoCreateState );

		// Returns the whole state object for a specified thread. If not present, auto-creates it if requested.
		platform::JNIThreadState * _getJNIThreadState( platform::JNIThreadID pJNIThreadID, bool pAutoCreateState );

		// Handles destruction of the thread state for a specified thread.
		void _onJNIThreadStateDestroyRequest( platform::JNIThreadID pJNIThreadID, bool pForceRelease );

	private:
		struct JavaVMInstancePrivateData;
		std::unique_ptr<JavaVMInstancePrivateData> _privateData;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_JAVA_VM_H__
