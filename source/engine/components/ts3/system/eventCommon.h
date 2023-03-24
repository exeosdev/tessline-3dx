
#ifndef __TS3_SYSTEM_EVENT_COMMON_H__
#define __TS3_SYSTEM_EVENT_COMMON_H__

#include "perfCounter.h"
#include <ts3/math/vectorOps.h>
#include <functional>

namespace ts3::system
{

	struct EventObject;

	class EventController;
	class EventDispatcher;
	class EventSource;

	/// @brief
	using EventHandler = std::function<bool( EventObject & )>;

	/// @brief
	using event_code_value_t = uint32;

	/// @brief
	enum class EEventBaseType : uint8
	{
		Undefined,
		App,
		Input,
		Window,
		_ReservedMax
	};

	/// @brief
	enum class EEventCategory : uint8
	{
		Undefined,
		AppActivity,
		InputGamepad,
		InputKeyboard,
		InputMouse,
		InputTouch,
		WindowUpdate,
		_ReservedMax
	};

	/// @brief
	enum class EEventCodeIndex : uint8
	{
		Undefined,
		AppActivityDisplayInit,
		AppActivityDisplayTerm,
		AppActivityFocusGained,
		AppActivityFocusLost,
		AppActivityPause,
		AppActivityResume,
		AppActivityStart,
		AppActivityStop,
		AppActivityQuit,
		AppActivityTerminate,
		InputGamepadAxis,
		InputGamepadButton,
		InputGamepadState,
		InputKeyboard,
		InputMouseButton,
		InputMouseMove,
		InputMouseScroll,
		InputTouchDown,
		InputTouchMove,
		InputTouchUp,
		WindowUpdateCreate,
		WindowUpdateDestroy,
		WindowUpdateFullscreen,
		WindowUpdateResize,
		WindowUpdateVisibility,
		_ReservedMax
	};

	constexpr uint8 CX_EVENT_CODE_CONTROL_KEY = 0xEC;

	constexpr auto CX_ENUM_EVENT_BASE_TYPE_COUNT = static_cast< size_t >( EEventBaseType::_ReservedMax );

	constexpr auto CX_ENUM_EVENT_CATEGORY_COUNT = static_cast< size_t >( EEventCategory::_ReservedMax );

	constexpr auto CX_ENUM_EVENT_CODE_INDEX_COUNT = static_cast< size_t >( EEventCodeIndex::_ReservedMax );

	/// @brief
	struct EvtBase
	{
		//
		event_code_value_t eventCode;

		//
		perf_counter_value_t timeStamp;
	};


	namespace cxdefs
	{

		inline constexpr event_code_value_t declareEventCode( EEventBaseType pEventBaseType, EEventCategory pEventCategory, EEventCodeIndex pEventCodeIndex )
		{
			return ( ( (uint32)CX_EVENT_CODE_CONTROL_KEY << 24 ) | ( (uint32)pEventBaseType << 16 ) | ( (uint32)pEventCategory << 8 ) | (uint32)pEventCodeIndex );
		}

		inline constexpr event_code_value_t declareEventCodeAppActivity( EEventCodeIndex pEventCodeIndex )
		{
			return declareEventCode( EEventBaseType::App, EEventCategory::AppActivity, pEventCodeIndex );
		}

		inline constexpr event_code_value_t declareEventCodeInputGamepad( EEventCodeIndex pEventCodeIndex )
		{
			return declareEventCode( EEventBaseType::Input, EEventCategory::InputGamepad, pEventCodeIndex );
		}

		inline constexpr event_code_value_t declareEventCodeInputKeyboard( EEventCodeIndex pEventCodeIndex )
		{
			return declareEventCode( EEventBaseType::Input, EEventCategory::InputKeyboard, pEventCodeIndex );
		}

		inline constexpr event_code_value_t declareEventCodeInputMouse( EEventCodeIndex pEventCodeIndex )
		{
			return declareEventCode( EEventBaseType::Input, EEventCategory::InputMouse, pEventCodeIndex );
		}

		inline constexpr event_code_value_t declareEventCodeInputTouch( EEventCodeIndex pEventCodeIndex )
		{
			return declareEventCode( EEventBaseType::Input, EEventCategory::InputTouch, pEventCodeIndex );
		}

		inline constexpr event_code_value_t declareEventCodeWindowUpdate( EEventCodeIndex pEventCodeIndex )
		{
			return declareEventCode( EEventBaseType::Window, EEventCategory::WindowUpdate, pEventCodeIndex );
		}

		inline constexpr uint8 getEventCodeControlKey( event_code_value_t pEventCode )
		{
			return static_cast<uint8>( ( pEventCode >> 24 ) & 0xFF );
		}

		inline constexpr EEventBaseType getEventCodeBaseType( event_code_value_t pEventCode )
		{
			return static_cast<EEventBaseType>( ( pEventCode >> 16 ) & 0xFF );
		}

		inline constexpr EEventCategory getEventCodeCategory( event_code_value_t pEventCode )
		{
			return static_cast<EEventCategory>( ( pEventCode >> 8 ) & 0xFF );
		}

		inline constexpr EEventCodeIndex getEventCodeCodeIndex( event_code_value_t pEventCode )
		{
			return static_cast<EEventCodeIndex>( pEventCode & 0xFF );
		}

		inline constexpr bool validateEventCode( event_code_value_t pEventCode )
		{
			return ( getEventCodeControlKey( pEventCode ) == CX_EVENT_CODE_CONTROL_KEY ) &&
			       ( getEventCodeBaseType( pEventCode ) < EEventBaseType::_ReservedMax ) &&
			       ( getEventCodeCategory( pEventCode ) < EEventCategory::_ReservedMax ) &&
			       ( getEventCodeCodeIndex( pEventCode ) < EEventCodeIndex::_ReservedMax );
		}

	}

	enum EEventCode : event_code_value_t
	{
		E_EVENT_CODE_UNDEFINED = 0,
		E_EVENT_CODE_APP_ACTIVITY_DISPLAY_INIT  = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityDisplayInit ),
		E_EVENT_CODE_APP_ACTIVITY_DISPLAY_TERM  = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityDisplayTerm ),
		E_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED  = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityFocusGained ),
		E_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST    = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityFocusLost ),
		E_EVENT_CODE_APP_ACTIVITY_PAUSE         = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityPause ),
		E_EVENT_CODE_APP_ACTIVITY_RESUME        = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityResume ),
		E_EVENT_CODE_APP_ACTIVITY_START         = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityStart ),
		E_EVENT_CODE_APP_ACTIVITY_STOP          = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityStop ),
		E_EVENT_CODE_APP_ACTIVITY_QUIT          = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityQuit ),
		E_EVENT_CODE_APP_ACTIVITY_TERMINATE     = cxdefs::declareEventCodeAppActivity( EEventCodeIndex::AppActivityTerminate ),

		E_EVENT_CODE_INPUT_GAMEPAD_AXIS    = cxdefs::declareEventCodeInputGamepad( EEventCodeIndex::InputGamepadAxis ),
		E_EVENT_CODE_INPUT_GAMEPAD_BUTTON  = cxdefs::declareEventCodeInputGamepad( EEventCodeIndex::InputGamepadButton ),
		E_EVENT_CODE_INPUT_GAMEPAD_STATE   = cxdefs::declareEventCodeInputGamepad( EEventCodeIndex::InputGamepadState ),

		E_EVENT_CODE_INPUT_KEYBOARD  = cxdefs::declareEventCodeInputKeyboard( EEventCodeIndex::InputKeyboard ),

		E_EVENT_CODE_INPUT_MOUSE_BUTTON  = cxdefs::declareEventCodeInputMouse( EEventCodeIndex::InputMouseButton ),
		E_EVENT_CODE_INPUT_MOUSE_MOVE    = cxdefs::declareEventCodeInputMouse( EEventCodeIndex::InputMouseMove ),
		E_EVENT_CODE_INPUT_MOUSE_SCROLL  = cxdefs::declareEventCodeInputMouse( EEventCodeIndex::InputMouseScroll ),

		E_EVENT_CODE_INPUT_TOUCH_DOWN  = cxdefs::declareEventCodeInputTouch( EEventCodeIndex::InputTouchDown ),
		E_EVENT_CODE_INPUT_TOUCH_MOVE  = cxdefs::declareEventCodeInputTouch( EEventCodeIndex::InputTouchMove ),
		E_EVENT_CODE_INPUT_TOUCH_UP    = cxdefs::declareEventCodeInputTouch( EEventCodeIndex::InputTouchUp ),

		E_EVENT_CODE_WINDOW_UPDATE_CREATE     = cxdefs::declareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateCreate ),
		E_EVENT_CODE_WINDOW_UPDATE_DESTROY    = cxdefs::declareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateDestroy ),
		E_EVENT_CODE_WINDOW_UPDATE_FULLSCREEN = cxdefs::declareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateFullscreen ),
		E_EVENT_CODE_WINDOW_UPDATE_RESIZE     = cxdefs::declareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateResize ),
		E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY = cxdefs::declareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateVisibility )
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_COMMON_H__
