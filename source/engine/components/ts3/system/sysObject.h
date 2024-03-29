
#ifndef __TS3_SYSTEM_SYS_OBJECT_H__
#define __TS3_SYSTEM_SYS_OBJECT_H__

#include "prerequisites.h"
#include <ts3/stdext/bitmaskAtomic.h>

namespace ts3::system
{

	enum ESysObjectStateFlags : uint32
	{
		E_SYS_OBJECT_STATE_FLAG_DESTROY_REQUEST_FLAG_SET_BIT = 0x4000,

		E_SYS_OBJECT_STATE_FLAG_DESTROY_REQUEST_PROCESSED_BIT = 0x8000,

		E_SYS_OBJECT_STATE_FLAG_INVALID_BIT = 0x8000
	};

    /// @brief Base class for all system classes
    ///
    /// This class is used for easy injection of useful stuff (debug logs, instance counting, lifetime control)
    /// into all classes created by the System Library. It also provides (and, thus, enforces!) SysContext
    /// access, so it doesn't have to be declared multiple times (for every separate class in the library).
    class SysObject : public DynamicInterface
    {
    public:
        // Public handle to the SysContext. This is always non-null and specified at construction time.
        system::SysContextHandle const mSysContext;

    public:
        explicit SysObject( system::SysContextHandle pSysContext );
        virtual ~SysObject();

		explicit operator bool() const noexcept
		{
			return true;
		}

		/// @brief Destroys the "system part" of the object, keeping the instance itself in a defined state.
		///
		/// This method explicitly requests the underlying system data/object to be destroyed. The meaning of this call
		/// depends on the actual class and operating system. In most cases:
		/// - when called for a Window, it acts as if the window has been closed (usually via EvtWindowUpdateDestroy),
		/// - when called for an OpenGLRenderContext, it flushes commands, unbinds the context and destroys it,
		/// - for both cases (Window and OpenGLRenderContext) removes them from being active event sources.
		/// Refer to the specific system object for more info.
		///
		/// Note, that there is hardly any common case in which this method should be called directly. It is automatically
		/// invoked when the last handle of an object goes out of scope and before calling this object's destructor.
		/// This method itself IS NOT a part of the dtor! Of course this fact automatically implies the following things:
		/// - if an object is managed by a raw pointer, this needs to be called before destroying that object
		/// - if an object should be destroyed "remotely" (e.g. custom OS-level stuff), this can be used to do that.
		bool destroySystemObject();

		TS3_ATTR_NO_DISCARD bool isStateMaskSet( Bitmask<ESysObjectStateFlags> pMask ) const
		{
			return _stateMask.isSet( pMask );
		}

		/// @brief Returns true if this object has been requested to be destroyed or false otherwise.
		/// A positive value indicates, that destroySystemObject() has been successfully called for this object.
		/// Since the destruction may require, for example, system events to be processed, an object may still be valid
		/// even though the request has been already made.
		TS3_ATTR_NO_DISCARD bool isDestroyRequestSet() const
		{
			return isStateMaskSet( E_SYS_OBJECT_STATE_FLAG_DESTROY_REQUEST_FLAG_SET_BIT );
		}

		TS3_ATTR_NO_DISCARD bool isValidSystemObject() const
		{
			return !isStateMaskSet( E_SYS_OBJECT_STATE_FLAG_INVALID_BIT );
		}

	protected:
		/// @brief Implements class-specific logic for releasing system-level resources.
		///
		/// Base method (from SysObject class) should ALWAYS be called at the end of the class-specific one.
		/// However, it is not mandatory to form a complete call chain for all subclasses. Classes are also
		/// free to decide at which point a base class version should be called.
		virtual void onDestroySystemObjectRequested();

		bool setDestroyRequestFlag()
		{
			return _stateMask.testAndSet( E_SYS_OBJECT_STATE_FLAG_DESTROY_REQUEST_FLAG_SET_BIT );
		}

		void setStateFlags( bool pSetOrUnset, Bitmask<ESysObjectStateFlags> pFlags )
		{
			_stateMask.setOrUnset( pFlags, pSetOrUnset );
		}

	private:
		AtomicBitmask<uint32> _stateMask;
    };

    struct SysObjectDeleter
    {
        void operator()( SysObject * pSysObject ) const
        {
            if( pSysObject )
            {
                pSysObject->destroySystemObject();

                delete pSysObject;
            }
        }
    };

    template <typename TObject, typename... TArgs>
    inline SysHandle<TObject> createSysObject( TArgs && ...pArgs )
    {
        return createDynamicInterfaceObjectWithDeleter<TObject>( SysObjectDeleter{}, std::forward<TArgs>( pArgs )... );
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_SYS_OBJECT_H__
