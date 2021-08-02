
#ifndef __TS3_SYSTEM_EVENT_DEFS_H__
#define __TS3_SYSTEM_EVENT_DEFS_H__

#include "perfCounter.h"
#include <functional>

#define TS3_ENUM_EVENT_CODE_CONTROL_KEY    0xEC000000U
#define TS3_ENUM_EVENT_CODE_BASE_TYPE_MASK 0x00FF0000U
#define TS3_ENUM_EVENT_CODE_CATEGORY_MASK  0x0000FF00U
#define TS3_ENUM_EVENT_CODE_INDEX_MASK     0x000000FFU

#define ExfEnumDeclareEventCode( pBaseType, pCategory, pIndex ) \
	( TS3_ENUM_EVENT_CODE_CONTROL_KEY | ( (uint32)( pBaseType ) << 16 ) | ( (uint32)( pCategory ) << 8 ) | (uint32)( pIndex ) )

#define ExfEnumDeclareEventCodeAppActivity( pIndex ) \
	ExfEnumDeclareEventCode( EEventBaseType::App, EEventCategory::AppActivity, pIndex )

#define ExfEnumDeclareEventCodeInputGamepad( pIndex ) \
	ExfEnumDeclareEventCode( EEventBaseType::Input, EEventCategory::InputGamepad, pIndex )

#define ExfEnumDeclareEventCodeInputKeyboard( pIndex ) \
	ExfEnumDeclareEventCode( EEventBaseType::Input, EEventCategory::InputKeyboard, pIndex )

#define ExfEnumDeclareEventCodeInputMouse( pIndex ) \
	ExfEnumDeclareEventCode( EEventBaseType::Input, EEventCategory::InputMouse, pIndex )

#define ExfEnumDeclareEventCodeInputTouch( pIndex ) \
	ExfEnumDeclareEventCode( EEventBaseType::Input, EEventCategory::InputTouch, pIndex )

#define ExfEnumDeclareEventCodeWindowUpdate( pIndex ) \
	ExfEnumDeclareEventCode( EEventBaseType::Window, EEventCategory::WindowUpdate, pIndex )

#define ExfEnumGetEventCodeBaseType( pEventCode ) \
	(EEventBaseType)( ( ( event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_BASE_TYPE_MASK ) >> 16 )

#define ExfEnumGetEventCodeCategory( pEventCode ) \
	( EEventCategory )( ( ( event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_CATEGORY_MASK ) >> 8 )

#define ExfEnumGetEventCodeIndex( pEventCode ) \
	( EEventCodeIndex )( ( event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_INDEX_MASK )

#define ExfEnumValidateEventCode( pEventCode ) \
	( ( ( event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_CONTROL_KEY ) == TS3_ENUM_EVENT_CODE_CONTROL_KEY )

namespace ts3
{
namespace system
{

	ts3DeclareHandle( EventSource );
	ts3DeclareHandle( EventController );
	ts3DeclareHandle( EventDispatcher );

	struct Event;

	/// @brief
	using EventHandler = std::function<bool( Event & )>;

	/// @brief
	using event_code_value_t = uint32;

	/// @brief
	enum class EEventBaseType : event_code_value_t
	{
		Undefined,
		App,
		Input,
		Window,
		_Reserved
	};

	/// @brief
	enum class EEventCategory : event_code_value_t
	{
		Undefined,
		AppActivity,
		InputGamepad,
		InputKeyboard,
		InputMouse,
		InputTouch,
		WindowUpdate,
		_Reserved
	};

	/// @brief
	enum class EEventCodeIndex : event_code_value_t
	{
		Undefined,
		AppActivityDisplayReady,
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
		InputKeyboardKey,
		InputMouseButton,
		InputMouseMove,
		InputMouseScroll,
		InputTouchDown,
		InputTouchMove,
		InputTouchUp,
		WindowUpdateClose,
		WindowUpdateFullscreen,
		WindowUpdateResize,
		WindowUpdateVisibility,
		_Reserved
	};

	inline constexpr auto cvEnumEventBaseTypeCount = static_cast< size_t >( EEventBaseType::_Reserved );
	inline constexpr auto cvEnumEventCategoryCount = static_cast< size_t >( EEventCategory::_Reserved );
	inline constexpr auto cvEnumEventCodeIndexCount = static_cast< size_t >( EEventCodeIndex::_Reserved );

	/// @brief
	enum EventControllerConfigFlagBits : uint32
	{
		E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
		E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
		E_EVENT_CONTROLLER_CONFIG_FLAGS_DEFAULT = E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT
	};

	/// @brief
	struct EventControllerConfig
	{
		//
		Bitmask<EventControllerConfigFlagBits> flags = E_EVENT_CONTROLLER_CONFIG_FLAGS_DEFAULT;
		//
		event_code_value_t mouseClickSequenceTimeoutMs = 100;
	};

	enum EEventCode : event_code_value_t
	{
		E_EVENT_CODE_UNDEFINED = 0,
		E_EVENT_CODE_APP_ACTIVITY_DISPLAY_READY = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityDisplayReady ),
		E_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityFocusGained ),
		E_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityFocusLost ),
		E_EVENT_CODE_APP_ACTIVITY_PAUSE = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityPause ),
		E_EVENT_CODE_APP_ACTIVITY_RESUME = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityResume ),
		E_EVENT_CODE_APP_ACTIVITY_START = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityStart ),
		E_EVENT_CODE_APP_ACTIVITY_STOP = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityStop ),
		E_EVENT_CODE_APP_ACTIVITY_QUIT = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityQuit ),
		E_EVENT_CODE_APP_ACTIVITY_TERMINATE = ExfEnumDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityTerminate ),
		E_EVENT_CODE_INPUT_GAMEPAD_AXIS = ExfEnumDeclareEventCodeInputGamepad( EEventCodeIndex::InputGamepadAxis ),
		E_EVENT_CODE_INPUT_GAMEPAD_BUTTON = ExfEnumDeclareEventCodeInputGamepad( EEventCodeIndex::InputGamepadButton ),
		E_EVENT_CODE_INPUT_GAMEPAD_STATE = ExfEnumDeclareEventCodeInputGamepad( EEventCodeIndex::InputGamepadState ),
		E_EVENT_CODE_INPUT_KEYBOARD_KEY = ExfEnumDeclareEventCodeInputKeyboard( EEventCodeIndex::InputKeyboardKey ),
		E_EVENT_CODE_INPUT_MOUSE_BUTTON = ExfEnumDeclareEventCodeInputMouse( EEventCodeIndex::InputMouseButton ),
		E_EVENT_CODE_INPUT_MOUSE_MOVE = ExfEnumDeclareEventCodeInputMouse( EEventCodeIndex::InputMouseMove ),
		E_EVENT_CODE_INPUT_MOUSE_SCROLL = ExfEnumDeclareEventCodeInputMouse( EEventCodeIndex::InputMouseScroll ),
		E_EVENT_CODE_INPUT_TOUCH_DOWN = ExfEnumDeclareEventCodeInputTouch( EEventCodeIndex::InputTouchDown ),
		E_EVENT_CODE_INPUT_TOUCH_MOVE = ExfEnumDeclareEventCodeInputTouch( EEventCodeIndex::InputTouchMove ),
		E_EVENT_CODE_INPUT_TOUCH_UP = ExfEnumDeclareEventCodeInputTouch( EEventCodeIndex::InputTouchUp ),
		E_EVENT_CODE_WINDOW_UPDATE_CLOSE = ExfEnumDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateClose ),
		E_EVENT_CODE_WINDOW_UPDATE_FULLSCREEN = ExfEnumDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateFullscreen ),
		E_EVENT_CODE_WINDOW_UPDATE_RESIZE = ExfEnumDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateResize ),
		E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY = ExfEnumDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateVisibility )
	};

	/// @brief
	struct EvtBase
	{
	public:
		//
		event_code_value_t eventCode;
		//
		perf_counter_value_t timeStamp;

	public:
		explicit EvtBase( event_code_value_t pEventCode )
		: eventCode( pEventCode )
		, timeStamp( 0 )
		{}
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_EVENT_DEFS_H__
