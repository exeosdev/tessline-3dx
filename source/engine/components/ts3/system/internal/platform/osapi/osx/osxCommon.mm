
#include "osxCommon.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
namespace ts3::system
{

	namespace platform
	{

		const char * osxQueryCGErrorMessage( CGError pCGError )
		{
			switch( pCGError )
			{
			case kCGErrorSuccess           : return "kCGErrorSuccess"           ;
			case kCGErrorFailure           : return "kCGErrorFailure"           ;
			case kCGErrorIllegalArgument   : return "kCGErrorIllegalArgument"   ;
			case kCGErrorInvalidConnection : return "kCGErrorInvalidConnection" ;
			case kCGErrorInvalidContext    : return "kCGErrorInvalidContext"    ;
			case kCGErrorCannotComplete    : return "kCGErrorCannotComplete"    ;
			case kCGErrorNotImplemented    : return "kCGErrorNotImplemented"    ;
			case kCGErrorRangeCheck        : return "kCGErrorRangeCheck"        ;
			case kCGErrorTypeCheck         : return "kCGErrorTypeCheck"         ;
			case kCGErrorInvalidOperation  : return "kCGErrorInvalidOperation"  ;
			case kCGErrorNoneAvailable     : return "kCGErrorNoneAvailable"     ;
			}
			return "<UNKNOWN>";
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_OSX
