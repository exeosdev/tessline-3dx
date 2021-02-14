
#ifndef __TS3_SYSTEM_EVENT_CORE_H__
#define __TS3_SYSTEM_EVENT_CORE_H__

#include "eventObject.h"
#include "systemContext.h"
#include <ts3/math/vector.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidEventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32EventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11EventCore.h"
#endif

namespace ts3
{

	// Represents invalid mouse position. Used as a default value for last position registered.
	constexpr math::Vec2i32 cvSysMousePosInvalid { cxInt32Max, cxInt32Max };

	enum ESysEventConfigFlags : uint32
	{
		E_SYS_EVENT_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
		E_SYS_EVENT_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_SYS_EVENT_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
		E_SYS_EVENT_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT = 0x1000
	};

	struct SysEventInputState
	{
		//
		SysContextNativeData * scNativeData = nullptr;
		//
		Bitmask<ESysEventConfigFlags> flags = 0u;
		//
		duration_value_t mouseClickSequenceTimeoutMs = 100;
		// Current state of the keyboard.
		SysKeyboardState keyboardState;
		// Last cursor position registered by the event system.
		math::Vec2i32 mouseLastRegPos = cvSysMousePosInvalid;
		// State of the mouse buttons. Used for motion events on systems which do not have reliable states (X11).
		Bitmask<SysMouseButtonFlagBits> mouseButtonStateMask = 0;
		// Last mouse button pressed. Used to detect multi-click sequences.
		SysMouseButtonID mouseLastPressButton = SysMouseButtonID::Unknown;
		// Current sequence length, i.e. number of clicks of the same button in a row.
		uint32 mouseClickSequenceLength = 1;
		// Timestamp of last registered mouse button press.
		sys_perf_counter_value_t mouseLastPressTimestamp = 0u;
	};

	class SysEventSource
	{
		friend class SysEventController;

	public:
		SysEventSourceNativeData & mEvtSrcNativeData;

		explicit SysEventSource( SysEventSourceNativeData & pEvtSrcNativeData ) noexcept
		: mEvtSrcNativeData( pEvtSrcNativeData )
		{}
	};

	class SysEventController : public SysBaseObject
	{
		friend class SysEventDispatcher;

	public:
		explicit SysEventController( SysContextHandle pSysContext ) noexcept;
		virtual ~SysEventController() noexcept;

		static SysEventControllerHandle create( SysContextHandle pSysContext );

		SysEventDispatcherHandle createEventDispatcher();

		void dispatchEvent( SysEvent & pEvent );

		void processEvent();
		void processQueue();
		void dispatchNextEvent();
		void dispatchNextEventWait();
		void dispatchQueuedEvents();
		void dispatchQueuedEventsWait();

		void addEventSource( SysEventSource & pEventSource );
		void removeEventSource( SysEventSource & pEventSource );

		void setActiveDispatcher( SysEventDispatcher & pDispatcher );
		void resetActiveDispatcher();

		TS3_PCL_ATTR_NO_DISCARD SysEventDispatcher & getActiveDispatcher() const;
		TS3_PCL_ATTR_NO_DISCARD SysEventDispatcher & getDefaultDispatcher() const;

	private:
		void _sysInitialize();
		void _sysRelease();
		void _sysAddEventSource( SysEventSource & pEventSource );
		void _sysRemoveEventSource( SysEventSource & pEventSource );
		void _sysDispatchNextEvent();
		void _sysDispatchNextEventWait();
		void _sysDispatchQueuedEvents();
		void _sysDispatchQueuedEventsWait();

	private:
		//
		SysEventDispatcherHandle _defaultDispatcher;
		//
		SysEventDispatcher * _activeDispatcher;
	};

	class SysEventDispatcher : public SysBaseObject
	{
		friend class SysEventController;

	public:
		SysEventControllerHandle const mEventController;

		SysEventDispatcher( SysEventControllerHandle pEventController ) noexcept;
		~SysEventDispatcher() noexcept;

		static SysEventDispatcherHandle create( SysEventControllerHandle pEventController );

		void bindEventHandler( ESysEventBaseType pBaseType, SysEventHandler pHandler );
		void bindEventHandler( ESysEventCategory pCategory, SysEventHandler pHandler );
		void bindEventHandler( ESysEventCodeIndex pCodeIndex, SysEventHandler pHandler );
		void bindDefaultEventHandler( SysEventHandler pHandler );

		void postEvent( SysEvent pEvent );
		void postSimpleEvent( sys_event_code_value_t pEventCode );
		void postEventAppQuit();
		void postEventAppTerminate();

		void setIdleProcessingMode( bool pIdle );

		bool checkIdleProcessingMode() const;

		SysEventInputState & getInputState();

	private:
		void internalPutEvent( SysEvent & pEvent );

		void _preProcessEvent( SysEvent & pEvent );
		void _putEvent( SysEvent & pEvent );

	private:
		using EventBaseTypeHandlerMap = std::array<SysEventHandler, cvSysEnumEventBaseTypeCount>;
		using EventCategoryHandlerMap = std::array<SysEventHandler, cvSysEnumEventCategoryCount>;
		using EventCodeIndexHandlerMap = std::array<SysEventHandler, cvSysEnumEventCodeIndexCount>;

		//
		SysEventInputState _inputState;
		//
		SysEventHandler _defaultHandler;
		// Array of handlers registered for EventBaseType.
		EventBaseTypeHandlerMap _handlerMapByBaseType;
		// Array of handlers registered for EventCategory.
		EventCategoryHandlerMap _handlerMapByCategory;
		// Array of handlers registered for EventCodeIndex (i.e. event code itself).
		EventCodeIndexHandlerMap _handlerMapByCodeIndex;
	};

}

#endif // __TS3_SYSTEM_EVENT_CORE_H__
