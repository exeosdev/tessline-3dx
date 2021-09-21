
#ifndef __TS3_SYSTEM_EVENT_COMMON_H__
#define __TS3_SYSTEM_EVENT_COMMON_H__

#include "perfCounter.h"
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
		_Reserved
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
		_Reserved
	};

	/// @brief
	enum class EEventCodeIndex : uint8
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

	constexpr uint8 CX_EVENT_CODE_CONTROL_KEY = 0xEC;

	constexpr auto CX_ENUM_EVENT_BASE_TYPE_COUNT = static_cast< size_t >( EEventBaseType::_Reserved );

	constexpr auto CX_ENUM_EVENT_CATEGORY_COUNT = static_cast< size_t >( EEventCategory::_Reserved );

	constexpr auto CX_ENUM_EVENT_CODE_INDEX_COUNT = static_cast< size_t >( EEventCodeIndex::_Reserved );

	/// @brief
	struct EvtBase
    {
    public:
        //
        event_code_value_t eventCode;
        //
        perf_counter_value_t timeStamp;

    public:
        explicit constexpr EvtBase( event_code_value_t pEventCode )
        : eventCode( pEventCode )
        , timeStamp( 0 )
        {}
    };

	/// @brief
	union EventCodeGen
    {
	    struct
	    {
	        uint8 uControlKey;
	        uint8 uEventBaseType;
	        uint8 uEventCategory;
	        uint8 uEventCodeIndex;
	    };

	    event_code_value_t eventCodeValue;

	    constexpr EventCodeGen()
	    : eventCodeValue( 0u )
	    {}

	    constexpr explicit EventCodeGen( event_code_value_t pCodeValue )
	    : eventCodeValue( pCodeValue )
	    {}

	    constexpr EventCodeGen( EEventBaseType pEventBaseType, EEventCategory pEventCategory, EEventCodeIndex pEventCodeIndex )
	    : uControlKey( CX_EVENT_CODE_CONTROL_KEY )
	    , uEventBaseType( static_cast<uint8>( pEventBaseType ) )
	    , uEventCategory( static_cast<uint8>( pEventCategory ) )
	    , uEventCodeIndex( static_cast<uint8>( pEventCodeIndex ) )
	    {}
    };

	inline constexpr event_code_value_t ecDeclareEventCode( EEventBaseType pEventBaseType, EEventCategory pEventCategory, EEventCodeIndex pEventCodeIndex )
	{
	    return ( ( (uint32)CX_EVENT_CODE_CONTROL_KEY << 24 ) | ( (uint32)pEventBaseType << 16 ) | ( (uint32)pEventCategory << 8 ) | (uint32)pEventCodeIndex );
	}

	inline constexpr event_code_value_t ecDeclareEventCodeAppActivity( EEventCodeIndex pEventCodeIndex )
	{
	    return ecDeclareEventCode( EEventBaseType::App, EEventCategory::AppActivity, pEventCodeIndex );
	}

	inline constexpr event_code_value_t ecDeclareEventCodeInputGamepad( EEventCodeIndex pEventCodeIndex )
	{
	    return ecDeclareEventCode( EEventBaseType::Input, EEventCategory::InputGamepad, pEventCodeIndex );
	}

	inline constexpr event_code_value_t ecDeclareEventCodeInputKeyboard( EEventCodeIndex pEventCodeIndex )
	{
	    return ecDeclareEventCode( EEventBaseType::Input, EEventCategory::InputKeyboard, pEventCodeIndex );
	}

	inline constexpr event_code_value_t ecDeclareEventCodeInputMouse( EEventCodeIndex pEventCodeIndex )
	{
	    return ecDeclareEventCode( EEventBaseType::Input, EEventCategory::InputMouse, pEventCodeIndex );
	}

	inline constexpr event_code_value_t ecDeclareEventCodeInputTouch( EEventCodeIndex pEventCodeIndex )
	{
	    return ecDeclareEventCode( EEventBaseType::Input, EEventCategory::InputTouch, pEventCodeIndex );
	}

	inline constexpr event_code_value_t ecDeclareEventCodeWindowUpdate( EEventCodeIndex pEventCodeIndex )
	{
	    return ecDeclareEventCode( EEventBaseType::Window, EEventCategory::WindowUpdate, pEventCodeIndex );
	}

	inline constexpr EEventBaseType ecGetEventCodeBaseType( event_code_value_t pEventCode )
	{
	    return static_cast<EEventBaseType>( pEventCode >> 16 );
	}

	inline constexpr EEventCategory ecGetEventCodeCategory( event_code_value_t pEventCode )
	{
	    return static_cast<EEventCategory>( pEventCode >> 8 );
	}

	inline constexpr EEventCodeIndex ecGetEventCodeCodeIndex( event_code_value_t pEventCode )
	{
	    return static_cast<EEventCodeIndex>( pEventCode & 0xFF );
	}

	inline constexpr bool ecValidateEventCode( event_code_value_t pEventCode )
	{
	    return  ( EventCodeGen( pEventCode ).uControlKey == CX_EVENT_CODE_CONTROL_KEY ) &&
	            ( EventCodeGen( pEventCode ).uEventBaseType < CX_ENUM_EVENT_BASE_TYPE_COUNT ) &&
	            ( EventCodeGen( pEventCode ).uEventCategory < CX_ENUM_EVENT_CATEGORY_COUNT ) &&
	            ( EventCodeGen( pEventCode ).uEventCodeIndex < CX_ENUM_EVENT_CODE_INDEX_COUNT );
	}

	enum EEventCode : event_code_value_t
	{
		E_EVENT_CODE_UNDEFINED = 0,
		E_EVENT_CODE_APP_ACTIVITY_DISPLAY_READY = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityDisplayReady ),
		E_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED  = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityFocusGained ),
		E_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST    = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityFocusLost ),
		E_EVENT_CODE_APP_ACTIVITY_PAUSE         = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityPause ),
		E_EVENT_CODE_APP_ACTIVITY_RESUME        = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityResume ),
		E_EVENT_CODE_APP_ACTIVITY_START         = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityStart ),
		E_EVENT_CODE_APP_ACTIVITY_STOP          = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityStop ),
		E_EVENT_CODE_APP_ACTIVITY_QUIT          = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityQuit ),
		E_EVENT_CODE_APP_ACTIVITY_TERMINATE     = ecDeclareEventCodeAppActivity( EEventCodeIndex::AppActivityTerminate ),

		E_EVENT_CODE_INPUT_GAMEPAD_AXIS         = ecDeclareEventCodeInputGamepad( EEventCodeIndex::InputGamepadAxis ),
		E_EVENT_CODE_INPUT_GAMEPAD_BUTTON       = ecDeclareEventCodeInputGamepad( EEventCodeIndex::InputGamepadButton ),
		E_EVENT_CODE_INPUT_GAMEPAD_STATE        = ecDeclareEventCodeInputGamepad( EEventCodeIndex::InputGamepadState ),

		E_EVENT_CODE_INPUT_KEYBOARD_KEY         = ecDeclareEventCodeInputKeyboard( EEventCodeIndex::InputKeyboardKey ),

		E_EVENT_CODE_INPUT_MOUSE_BUTTON         = ecDeclareEventCodeInputMouse( EEventCodeIndex::InputMouseButton ),
		E_EVENT_CODE_INPUT_MOUSE_MOVE           = ecDeclareEventCodeInputMouse( EEventCodeIndex::InputMouseMove ),
		E_EVENT_CODE_INPUT_MOUSE_SCROLL         = ecDeclareEventCodeInputMouse( EEventCodeIndex::InputMouseScroll ),

		E_EVENT_CODE_INPUT_TOUCH_DOWN           = ecDeclareEventCodeInputTouch( EEventCodeIndex::InputTouchDown ),
		E_EVENT_CODE_INPUT_TOUCH_MOVE           = ecDeclareEventCodeInputTouch( EEventCodeIndex::InputTouchMove ),
		E_EVENT_CODE_INPUT_TOUCH_UP             = ecDeclareEventCodeInputTouch( EEventCodeIndex::InputTouchUp ),

		E_EVENT_CODE_WINDOW_UPDATE_CLOSE        = ecDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateClose ),
		E_EVENT_CODE_WINDOW_UPDATE_FULLSCREEN   = ecDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateFullscreen ),
		E_EVENT_CODE_WINDOW_UPDATE_RESIZE       = ecDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateResize ),
		E_EVENT_CODE_WINDOW_UPDATE_VISIBILITY   = ecDeclareEventCodeWindowUpdate( EEventCodeIndex::WindowUpdateVisibility )
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_COMMON_H__
