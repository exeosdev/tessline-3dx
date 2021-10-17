
#include "javaVM.h"
#include "javaNativeInterface.h"
#include <map>

namespace ts3::system
{

    struct JavaVMInstance::JavaVMInstancePrivateData
    {
        std::map<platform::JNIThreadID, platform::JNIThreadState *> jniThreadStateMap;
    };


    JavaVMInstance::JavaVMInstance( JavaVM * pJavaVM )
    : mJavaVM( pJavaVM )
    {}

    JavaVMInstance::~JavaVMInstance() noexcept = default;

    JavaNativeInterfacePtr JavaVMInstance::acquireJNI( platform::JNIThreadID pJNIThreadID )
    {
        auto currentThreadID = std::this_thread::get_id();

        // Thread state can be auto created for the calling thread - and the calling thread only.
        // It involves a call to VM's AttachCurrentThread(), which works only for the current thread.
        bool autoCreateState = ( currentThreadID == pJNIThreadID );

        if( auto * jniObject = _acquireJNI( pJNIThreadID, autoCreateState ) )
        {
            return JavaNativeInterfacePtr( jniObject, platform::JavaNativeInterfaceDeleter{} );
        }

        return nullptr;
    }

    JavaNativeInterfacePtr JavaVMInstance::acquireJNIForCurrentThread()
    {
        auto currentThreadID = std::this_thread::get_id();
        if( auto * jniObject = _acquireJNI( currentThreadID, true ) )
        {
            return JavaNativeInterfacePtr( jniObject, platform::JavaNativeInterfaceDeleter{} );
        }
        return nullptr;
    }

    void JavaVMInstance::setCurrentThreadAutoReleaseState( bool pAutoRelease )
    {
        auto currentThreadID = std::this_thread::get_id();
        auto * jniThreadState = _getJNIThreadState( currentThreadID, true );
        jniThreadState->flags.set( platform::E_JNI_THREAD_STATE_AUTO_RELEASE_BIT );
    }

    void JavaVMInstance::releaseCurrentThreadState()
    {
        auto currentThreadID = std::this_thread::get_id();

        // Call the destroy request with force=true (this in the explicit release function).
        // Note, that this call may still fail if the reference counter has not reached 0.
        _onJNIThreadStateDestroyRequest( currentThreadID, true );
    }

    void JavaVMInstance::onJavaNativeInterfacePtrDestroy( JavaNativeInterface * pJNI )
    {
        // This function is called within ~JavaNativeInterface().

        // Retrieve the thread state using JNI's thread id. This should never return nullptr,
        // because there is at least one remaining JNI instance (the one being currently destroyed).
        auto * jniThreadState = _getJNIThreadState( pJNI->mJNIThreadID, false );
        if( !jniThreadState )
        {
            ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER,
                          "onJavaNativeInterfacePtrDestroy(): State mismatch: thread state not found" );
        }

        auto refCounter = jniThreadState->jniRefCounter.fetch_sub( 1, std::memory_order_acq_rel );
        if( refCounter == 1 )
        {
            // If this was the last active reference, we may want to release the whole state,
            // depending on the configuration. But this can be done only if the current thread
            // is the one that owns the state.
            auto currentThreadID = std::this_thread::get_id();
            if( pJNI->mJNIThreadID == currentThreadID )
            {
                // Call the destroy request, do not force deletion (it will be automatically
                // released if E_JNI_THREAD_STATE_AUTO_RELEASE_BIT has been set for this thread).
                _onJNIThreadStateDestroyRequest( pJNI->mJNIThreadID, false );
            }
        }
    }

    JavaNativeInterface * JavaVMInstance::_acquireJNI( platform::JNIThreadID pJNIThreadID, bool pAutoCreateState )
    {
        if( auto * jniThreadState = _getJNIThreadState( pJNIThreadID, pAutoCreateState ) )
        {
            jniThreadState->jniRefCounter.fetch_add( 1, std::memory_order_acq_rel );
            return jniThreadState->jniObject;
        }
        return nullptr;
    }

    platform::JNIThreadState * JavaVMInstance::_getJNIThreadState( platform::JNIThreadID pJNIThreadID, bool pAutoCreateState )
    {
        auto jniThreadStateIter = _privateData->jniThreadStateMap.find( pJNIThreadID );
        if( ( jniThreadStateIter == _privateData->jniThreadStateMap.end() ) && pAutoCreateState )
        {
            auto currentThreadID = std::this_thread::get_id();
            if( pJNIThreadID != currentThreadID )
            {
                ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER,
                              "acquireJNIEnv(): JNIThreadState can be only initialized by the thread itself" );
            }

            auto jniThreadState = std::make_unique<platform::JNIThreadState>();
            JNIEnv * jniEnv = nullptr;

            auto threadAttachResult = mJavaVM->AttachCurrentThread( &jniEnv, nullptr );
            if( threadAttachResult != JNI_OK )
            {
                ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER,
                              "JavaVM::AttachCurrentThread() has failed" );
            }

            jniThreadState->jniEnvPtr = jniEnv;
            jniThreadState->jniThreadID = pJNIThreadID;
            jniThreadState->jniObject = new JavaNativeInterface( *this, pJNIThreadID, jniEnv );

            auto insertResult = _privateData->jniThreadStateMap.insert( { pJNIThreadID, jniThreadState.release() } );
            jniThreadStateIter = insertResult.first;
        }

        platform::JNIThreadState * jniThreadState = nullptr;

        if( jniThreadStateIter != _privateData->jniThreadStateMap.end() )
        {
            ts3DebugAssert( jniThreadStateIter->second->jniObject != nullptr );
            jniThreadState = jniThreadStateIter->second;
        }

        return jniThreadState;
    }

    void JavaVMInstance::_onJNIThreadStateDestroyRequest( platform::JNIThreadID pJNIThreadID, bool pForceRelease )
    {
        auto jniThreadStateIter = _privateData->jniThreadStateMap.find( pJNIThreadID );
        if( jniThreadStateIter != _privateData->jniThreadStateMap.end() )
        {
            auto currentThreadID = std::this_thread::get_id();
            if( pJNIThreadID != currentThreadID )
            {
                ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER,
                              "acquireJNIEnv(): JNIThreadState can be only destroyed by the thread itself" );
            }

            auto * jniThreadState = jniThreadStateIter->second;
            auto refCounter = jniThreadState->jniRefCounter.load( std::memory_order_acquire );

            if( refCounter > 0 )
            {
                ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER,
                              "_onJNIThreadStateDestroyRequest(): refCounter > 0" );
            }

            if( jniThreadState->flags.isSet( platform::E_JNI_THREAD_STATE_AUTO_RELEASE_BIT ) || pForceRelease )
            {
                std::unique_ptr<platform::JNIThreadState> jniThreadStatePtr{ jniThreadState };

                mJavaVM->DetachCurrentThread();

                _privateData->jniThreadStateMap.erase( jniThreadStateIter );

                delete jniThreadState->jniObject;
            }
        }
    }


    namespace platform
    {

        void JavaNativeInterfaceDeleter::operator()( JavaNativeInterface * pJNI ) const
        {
            auto * jvmInstance = pJNI->mJVMInstance;
            jvmInstance->onJavaNativeInterfacePtrDestroy( pJNI );
        }

    }

} // namespace ts3::system
