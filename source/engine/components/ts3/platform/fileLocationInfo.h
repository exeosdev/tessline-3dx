
#pragma once

#ifndef __TS3_PLATFORM_FILE_LOCATION_INFO_H__
#define __TS3_PLATFORM_FILE_LOCATION_INFO_H__

#include "platform.h"
#include <cstring>
#include <string>

namespace ts3
{

	namespace cxdefs
	{

		/// @brief Represents empty char* literal.
		inline constexpr const char * STR_CHAR_EMPTY = "";

		/// @brief Represents empty wchar_t* literal.
		inline constexpr const wchar_t * STR_WCHAR_EMPTY = L"";

	}

	struct FileLocationInfo
	{
	public:
		const char * function;
		const char * filename;
		int32 lineNumber;

	public:
		FileLocationInfo( const FileLocationInfo & ) = default;
		FileLocationInfo & operator=( const FileLocationInfo & ) = default;

		FileLocationInfo()
		: function( cxdefs::STR_CHAR_EMPTY )
		, filename( cxdefs::STR_CHAR_EMPTY )
		, lineNumber( 0 )
		{}

		FileLocationInfo( const char * pFunction, const char * pFilename, int32 pLineNumber )
		: function( pFunction )
		, filename( pFilename )
		, lineNumber( pLineNumber )
		{}

		TS3_PCL_ATTR_FUNC_NO_DISCARD std::string toString() const;
	};

	inline std::string toString( const FileLocationInfo & pLocationInfo )
	{
		return pLocationInfo.toString();
	}

	/// @brief Macro for shorter replacement of location info for a current source line.
	#define ts3CurrentFileLocationInfo() ::ts3::FileLocationInfo{ __FUNCTION__, __FILE__, __LINE__ }

}

#endif // __TS3_PLATFORM_FILE_LOCATION_INFO_H__
