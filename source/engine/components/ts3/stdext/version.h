
#pragma once

#include "prerequisites.h"

#define TS3_VERSION_MAJOR    0
#define TS3_VERSION_MINOR    0
#define TS3_VERSION_RELEASE  0
#define TS3_VERSION_BUILD    0

#define TS3_VER_PRODUCTVERSION      0,0,0,0
#define TS3_VER_PRODUCTVERSION_STR  "0.0.0.0"

#define TS3_VER_COMPANY_STR      "Exeos Software Development"
#define TS3_VER_COPYRIGHT_STR    "Copyright (c) 2020 Mateusz Grzejek"
#define TS3_VER_PRODUCTNAME_STR  "Exeos Framework"

namespace ts3
{

	/// @brief Representation of a version in MAJOR.MINOR format.
	union Version
	{
	public:
		struct
		{
			uint16 major;
			uint16 minor;
		};

		uint32 hash;

	public:
		TS3_PCL_ATTR_NO_DISCARD std::string toString() const;
	};

	inline constexpr Version cvVersionInvalid{ cxUint16Max, cxUint16Max };
	inline constexpr Version cvVersionUnknown{ 0, 0 };

	inline bool operator==( const Version & pLhs, const Version & pRhs )
	{
		return ( pLhs.major == pRhs.major ) && ( pLhs.minor == pRhs.minor );
	}

	inline bool operator!=( const Version & pLhs, const Version & pRhs )
	{
		return ( pLhs.major != pRhs.major ) || ( pLhs.minor != pRhs.minor );
	}

	inline bool operator>( const Version & pLhs, const Version & pRhs )
	{
		return ( pLhs.major > pRhs.major ) || ( ( pLhs.major == pRhs.major ) && ( pLhs.minor > pRhs.minor ) );
	}

	inline bool operator>=( const Version & pLhs, const Version & pRhs )
	{
		return ( pLhs.major >= pRhs.major ) || ( ( pLhs.major == pRhs.major ) && ( pLhs.minor >= pRhs.minor ) );
	}

	inline bool operator<( const Version & pLhs, const Version & pRhs )
	{
		return ( pLhs.major < pRhs.major ) || ( ( pLhs.major == pRhs.major ) && ( pLhs.minor < pRhs.minor ) );
	}

	inline bool operator<=( const Version & pLhs, const Version & pRhs )
	{
		return ( pLhs.major <= pRhs.major ) || ( ( pLhs.major == pRhs.major ) && ( pLhs.minor <= pRhs.minor ) );
	}

}
