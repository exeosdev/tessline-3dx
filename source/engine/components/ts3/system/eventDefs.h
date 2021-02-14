
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
	ExfEnumDeclareEventCode( ESysEventBaseType::App, ESysEventCategory::AppActivity, pIndex )

#define ExfEnumDeclareEventCodeInputGamepad( pIndex ) \
	ExfEnumDeclareEventCode( ESysEventBaseType::Input, ESysEventCategory::InputGamepad, pIndex )

#define ExfEnumDeclareEventCodeInputKeyboard( pIndex ) \
	ExfEnumDeclareEventCode( ESysEventBaseType::Input, ESysEventCategory::InputKeyboard, pIndex )

#define ExfEnumDeclareEventCodeInputMouse( pIndex ) \
	ExfEnumDeclareEventCode( ESysEventBaseType::Input, ESysEventCategory::InputMouse, pIndex )

#define ExfEnumDeclareEventCodeInputTouch( pIndex ) \
	ExfEnumDeclareEventCode( ESysEventBaseType::Input, ESysEventCategory::InputTouch, pIndex )

#define ExfEnumDeclareEventCodeWindowUpdate( pIndex ) \
	ExfEnumDeclareEventCode( ESysEventBaseType::Window, ESysEventCategory::WindowUpdate, pIndex )

#define ExfEnumGetEventCodeBaseType( pEventCode ) \
	(ESysEventBaseType)( ( ( sys_event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_BASE_TYPE_MASK ) >> 16 )

#define ExfEnumGetEventCodeCategory( pEventCode ) \
	( ESysEventCategory )( ( ( sys_event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_CATEGORY_MASK ) >> 8 )

#define ExfEnumGetEventCodeIndex( pEventCode ) \
	( ESysEventCodeIndex )( ( sys_event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_INDEX_MASK )

#define ExfEnumValidateEventCode( pEventCode ) \
	( ( ( sys_event_code_value_t )( pEventCode ) & TS3_ENUM_EVENT_CODE_CONTROL_KEY ) == TS3_ENUM_EVENT_CODE_CONTROL_KEY )

namespace ts3
{

	ts3DeclareSysHandle( SysEventSource );
	ts3DeclareSysHandle( SysEventController );
	ts3DeclareSysHandle( SysEventDispatcher );

	struct SysEvent;

	/// @brief
	using SysEventHandler = std::function<bool( SysEvent & )>;

	/// @brief
	using sys_event_code_value_t = uint32;

	/// @brief
	enum class ESysEventBaseType : sys_event_code_value_t
	{
		Undefined,
		App,
		Input,
		Window,
		_Reserved
	};

	/// @brief
	enum class ESysEventCategory : sys_event_code_value_t
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
	enum class ESysEventCodeIndex : sys_event_code_value_t
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

	inline constexpr auto cvSysEnumEventBaseTypeCount = static_cast< size_t >( ESysEventBaseType::_Reserved );
	inline constexpr auto cvSysEnumEventCategoryCount = static_cast< size_t >( ESysEventCategory::_Reserved );
	inline constexpr auto cvSysEnumEventCodeIndexCount = static_cast< size_t >( ESysEventCodeIndex::_Reserved );

	/// @brief
	enum SysEventControllerConfigFlagBits : uint32
	{
		E_SYS_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
		E_SYS_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_SYS_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
		E_SYS_EVENT_CONTROLLER_CONFIG_FLAGS_DEFAULT = E_SYS_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT
	};

	/// @brief
	struct SysEventControllerConfig
	{
		//
		Bitmask<SysEventControllerConfigFlagBits> flags = E_SYS_EVENT_CONTROLLER_CONFIG_FLAGS_DEFAULT;
		//
		sys_event_code_value_t mouseClickSequenceTimeoutMs = 100;
	};

	enum ESysEventCode : sys_event_code_value_t
	{
		E_SYS_EVENT_CODE_UNDEFINED = 0,
		E_SYS_EVENT_CODE_APP_ACTIVITY_DISPLAY_READY = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityDisplayReady ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityFocusGained ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityFocusLost ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_PAUSE = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityPause ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_RESUME = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityResume ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_START = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityStart ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_STOP = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityStop ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_QUIT = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityQuit ),
		E_SYS_EVENT_CODE_APP_ACTIVITY_TERMINATE = ExfEnumDeclareEventCodeAppActivity( ESysEventCodeIndex::AppActivityTerminate ),
		E_SYS_EVENT_CODE_INPUT_GAMEPAD_AXIS = ExfEnumDeclareEventCodeInputGamepad( ESysEventCodeIndex::InputGamepadAxis ),
		E_SYS_EVENT_CODE_INPUT_GAMEPAD_BUTTON = ExfEnumDeclareEventCodeInputGamepad( ESysEventCodeIndex::InputGamepadButton ),
		E_SYS_EVENT_CODE_INPUT_GAMEPAD_STATE = ExfEnumDeclareEventCodeInputGamepad( ESysEventCodeIndex::InputGamepadState ),
		E_SYS_EVENT_CODE_INPUT_KEYBOARD_KEY = ExfEnumDeclareEventCodeInputKeyboard( ESysEventCodeIndex::InputKeyboardKey ),
		E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON = ExfEnumDeclareEventCodeInputMouse( ESysEventCodeIndex::InputMouseButton ),
		E_SYS_EVENT_CODE_INPUT_MOUSE_MOVE = ExfEnumDeclareEventCodeInputMouse( ESysEventCodeIndex::InputMouseMove ),
		E_SYS_EVENT_CODE_INPUT_MOUSE_SCROLL = ExfEnumDeclareEventCodeInputMouse( ESysEventCodeIndex::InputMouseScroll ),
		E_SYS_EVENT_CODE_INPUT_TOUCH_DOWN = ExfEnumDeclareEventCodeInputTouch( ESysEventCodeIndex::InputTouchDown ),
		E_SYS_EVENT_CODE_INPUT_TOUCH_MOVE = ExfEnumDeclareEventCodeInputTouch( ESysEventCodeIndex::InputTouchMove ),
		E_SYS_EVENT_CODE_INPUT_TOUCH_UP = ExfEnumDeclareEventCodeInputTouch( ESysEventCodeIndex::InputTouchUp ),
		E_SYS_EVENT_CODE_WINDOW_UPDATE_CLOSE = ExfEnumDeclareEventCodeWindowUpdate( ESysEventCodeIndex::WindowUpdateClose ),
		E_SYS_EVENT_CODE_WINDOW_UPDATE_FULLSCREEN = ExfEnumDeclareEventCodeWindowUpdate( ESysEventCodeIndex::WindowUpdateFullscreen ),
		E_SYS_EVENT_CODE_WINDOW_UPDATE_RESIZE = ExfEnumDeclareEventCodeWindowUpdate( ESysEventCodeIndex::WindowUpdateResize ),
		E_SYS_EVENT_CODE_WINDOW_UPDATE_VISIBILITY = ExfEnumDeclareEventCodeWindowUpdate( ESysEventCodeIndex::WindowUpdateVisibility )
	};

	/// @brief
	struct SysEvtBase
	{
	public:
		//
		sys_event_code_value_t eventCode;
		//
		sys_perf_counter_value_t timeStamp;

	public:
		explicit SysEvtBase( sys_event_code_value_t pEventCode )
		: eventCode( pEventCode )
		, timeStamp( 0 )
		{}
	};

}

#endif // __TS3_SYSTEM_EVENT_DEFS_H__
