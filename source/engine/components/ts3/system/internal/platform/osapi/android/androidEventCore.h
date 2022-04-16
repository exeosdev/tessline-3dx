
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_CORE_H__

#include "androidCommon.h"
#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>

namespace ts3::system
{

	namespace platform
	{

		enum class AndroidNativeEventType : enum_default_value_t
		{
			AppCommand,
			Input,
			Unknown
		};

		enum : int32_t
		{
			E_ANDROID_EVT_CMD_USER_DESTROY_REQUESTED = 0xFF00,
		};

		inline constexpr int32_t CX_ANDROID_EVENT_LOOPER_WAIT_TIMEOUT_IMMEDIATE = 0;

		inline constexpr int32_t CX_ANDROID_EVENT_LOOPER_WAIT_TIMEOUT_INFINITY = -1;

		struct NativeEventType
		{
			AndroidNativeEventType type;
			union
			{
				int32 eAppCommand;
				AInputEvent * eInputEvent;
			};
		};

		using AndroidNativeEvent = NativeEventType;

	}

	class AndroidEventController : public AndroidNativeObject<EventController, platform::AndroidNativeDataCommon>
	{
	public:
		AndroidEventController( SysContextHandle pSysContext );
		virtual ~AndroidEventController() noexcept;

		using EventController::getEventSystemInternalConfig;
		using EventController::getEventDispatcherInputState;

	private:
		/// @override EventController::_nativeRegisterEventSource
		virtual void _nativeRegisterEventSource( EventSource & pEventSource ) override final;

		/// @override EventController::_nativeUnregisterEventSource
		virtual void _nativeUnregisterEventSource( EventSource & pEventSource ) override final;

		/// @override EventController::_nativeUpdateSysQueue
		virtual bool _nativeUpdateSysQueue() override final;

		/// @override EventController::_nativeUpdateSysQueueWait
		virtual bool _nativeUpdateSysQueueWait() override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_EVENT_CORE_H__
