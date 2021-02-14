
#pragma once

#ifndef __TS3_CORE_CORE_ENUMS_H__
#define __TS3_CORE_CORE_ENUMS_H__

namespace ts3
{

	enum class EActiveState : enum_default_value_t
	{
		Disabled = 0,
		Enabled = 1
	};

	enum EAccessModeFlags : uint32
	{
		E_ACCESS_MODE_FLAG_READ_BIT = 0x0001,
		E_ACCESS_MODE_FLAG_WRITE_BIT = 0x0002,
		E_ACCESS_MODE_FLAGS_FULL_ACCESS = E_ACCESS_MODE_FLAG_READ_BIT | E_ACCESS_MODE_FLAG_WRITE_BIT,
		E_ACCESS_MODE_FLAGS_NONE = 0
	};

}

#endif // __TS3_CORE_CORE_ENUMS_H__
