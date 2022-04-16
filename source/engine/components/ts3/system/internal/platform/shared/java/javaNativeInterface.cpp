
#include "javaNativeInterface.h"

namespace ts3::system
{

	JavaNativeInterface::JavaNativeInterface( JavaVMInstance & pJVMInstance,
	                                          platform::JNIThreadID pJNIThreadID,
	                                          platform::JNIEnvPtr pJNIEnvPtr )
	: mJVMInstance( &pJVMInstance )
	, mJNIThreadID( pJNIThreadID )
	, mJNIEnv( pJNIEnvPtr )
	{}

	JavaNativeInterface::~JavaNativeInterface() noexcept = default;

	void JavaNativeInterface::nativeActivitySetRequestedOrientation( jobject pNativeActivity,
	                                                                 platform::EAndroidScreenOrientation pOrientation )
	{
		auto * jNativeActivityClass = mJNIEnv->GetObjectClass( pNativeActivity );
		auto * jMethod = mJNIEnv->GetMethodID( jNativeActivityClass, "setRequestedOrientation", "(I)V" );
		mJNIEnv->CallVoidMethod( pNativeActivity, jMethod, static_cast<int>( pOrientation ) );
	}

} // namespace ts3::system
