
#pragma once

#ifndef __STDX_VERSION_H__
#define __STDX_VERSION_H__

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

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#  pragma clang diagnostic ignored "-Wnested-anon-types"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( push )
#  pragma warning( disable: 4201 )  // 'Nonstandard extension used: nameless struct/union'
#endif

namespace ts3
{

	/// @brief Representation of a version in 'major.minor' format.
	union Version
	{
	public:
		struct
		{
			/// Major component of the version number.
			uint16 major;

			/// Minor component of the version number.
			uint16 minor;
		};

		/// 32-bit hash of the version, which is assembled by combining both uint16 components into a single 32-bit value.
		uint32 hash;

	public:
		/// @brief Returns the string representation of the version.
		/// @return The string representation of the version, formatted as "major.minor".
		TS3_PCL_ATTR_FUNC_NO_DISCARD std::string toString() const
		{
			return std::to_string( major ) + "." + std::to_string( minor );
		}
	};

	/// @brief An invalid version constant - both major and minor components are set to UINT16_MAX.
	inline constexpr Version CX_VERSION_INVALID{ CX_UINT16_MAX, CX_UINT16_MAX };

	/// @brief An unknown version constant - both major and minor components are set to 0.
	inline constexpr Version CX_VERSION_UNKNOWN{ 0, 0 };

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

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __STDX_VERSION_H__
