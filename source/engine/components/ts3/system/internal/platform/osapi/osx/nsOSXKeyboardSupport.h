
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_NS_KEYBOARD_SUPPORT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_NS_KEYBOARD_SUPPORT_H__

#include "nsCommon.h"
#include <ts3/system/eventDefInputKeyboard.h>

namespace ts3::system
{

	enum EOSXModifierKeyMapFlags : uint32
	{
		E_OSX_MODIFIER_KEY_MAP_FLAG_COMMAND_LEFT = 8,
		E_OSX_MODIFIER_KEY_MAP_FLAG_COMMAND_RIGHT = 16,
		E_OSX_MODIFIER_KEY_MAP_FLAG_CONTROL_LEFT = 1,
		E_OSX_MODIFIER_KEY_MAP_FLAG_CONTROL_RIGHT = 8192,
		E_OSX_MODIFIER_KEY_MAP_FLAG_OPTION_LEFT = 32,
		E_OSX_MODIFIER_KEY_MAP_FLAG_OPTION_RIGHT = 64,
		E_OSX_MODIFIER_KEY_MAP_FLAG_SHIFT_LEFT = 2,
		E_OSX_MODIFIER_KEY_MAP_FLAG_SHIFT_RIGHT = 4,
		E_OSX_MODIFIER_KEY_MAP_MASK_COMMAND_ANY = E_OSX_MODIFIER_KEY_MAP_FLAG_COMMAND_LEFT | E_OSX_MODIFIER_KEY_MAP_FLAG_COMMAND_RIGHT,
		E_OSX_MODIFIER_KEY_MAP_MASK_CONTROL_ANY = E_OSX_MODIFIER_KEY_MAP_FLAG_CONTROL_LEFT | E_OSX_MODIFIER_KEY_MAP_FLAG_CONTROL_RIGHT,
		E_OSX_MODIFIER_KEY_MAP_MASK_OPTION_ANY = E_OSX_MODIFIER_KEY_MAP_FLAG_OPTION_LEFT | E_OSX_MODIFIER_KEY_MAP_FLAG_OPTION_RIGHT,
		E_OSX_MODIFIER_KEY_MAP_MASK_SHIFT_ANY = E_OSX_MODIFIER_KEY_MAP_FLAG_SHIFT_LEFT | E_OSX_MODIFIER_KEY_MAP_FLAG_SHIFT_RIGHT,
	};

	Bitmask<EKeyModifierFlags> osxTranslateModifierKeyFlags( Bitmask<NSEventModifierFlags> pModifierFlags )
	{
		Bitmask<EKeyModifierFlags> resultMask = 0;

		const auto deviceIndependentFlags = makeBitmask<uint32>( pModifierFlags & NSEventModifierFlagDeviceIndependentFlagsMask );
		const auto deviceSpecificFlags = makeBitmask<uint32>( pModifierFlags & ( 0xFFFF ) );

		if( deviceIndependentFlags.isSet( NSEventModifierFlagCommand ) )
		{
			if( deviceSpecificFlags.isSetAnyOf( E_OSX_MODIFIER_KEY_MAP_FLAG_COMMAND_RIGHT ) )
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_ALT_RIGHT_BIT );
			}
			else
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_ALT_LEFT_BIT );
			}
		}

		if( deviceIndependentFlags.isSet( NSEventModifierFlagControl ) )
		{
			if( deviceSpecificFlags.isSetAnyOf( E_OSX_MODIFIER_KEY_MAP_FLAG_CONTROL_RIGHT ) )
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_CONTROL_RIGHT_BIT );
			}
			else
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_CONTROL_LEFT_BIT );
			}
		}

		if( deviceIndependentFlags.isSet( NSEventModifierFlagShift ) )
		{
			if( deviceSpecificFlags.isSetAnyOf( E_OSX_MODIFIER_KEY_MAP_FLAG_SHIFT_RIGHT ) )
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_SHIFT_RIGHT_BIT );
			}
			else
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_SHIFT_LEFT_BIT );
			}
		}

		if( deviceIndependentFlags.isSet( NSEventModifierFlagOption ) )
		{
			if( deviceSpecificFlags.isSetAnyOf( E_OSX_MODIFIER_KEY_MAP_FLAG_OPTION_RIGHT ) )
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_GUI_RIGHT_BIT );
			}
			else
			{
				resultMask.set( E_KEY_MODIFIER_FLAG_GUI_LEFT_BIT );
			}
		}

		return resultMask;
	}

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_NS_KEYBOARD_SUPPORT_H__
