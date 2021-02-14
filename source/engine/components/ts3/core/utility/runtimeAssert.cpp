
#include "runtimeAssert.h"

namespace ts3
{

	void _assertionFail( const char * pConditionStr, const FileLocationInfo & pLocationInfo )
	{
		std::string message{ "Debug assertion failed!" };
		message.append( "\nWhere: " );
		message.append( pLocationInfo.toString() );
		message.append( "\nCondition: " );
		message.append( pConditionStr );

	#if ( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_WINDESKTOP )
		message.append( "\n\nAbort: terminate\nRetry: break here\nIgnore: skip" );
		int result = ::MessageBoxA( nullptr, message.data(), "Runtime interrupt", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_APPLMODAL );
		if( result == IDABORT )
		{
			throw 0;
		}
		else if( result == IDRETRY )
		{
		}
	#else
		DebugInterface::_defaultPrintHandler( message.c_str(), message.length() );
	#endif
	}

	void _interrupt( const FileLocationInfo & pLocationInfo )
	{
		std::string message{ "Debug interrupt triggered!" };
		message.append( "\nWhere: " );
		message.append( pLocationInfo.toString() );

	#if ( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_WINDESKTOP )
		message.append( "\n\nAbort: terminate\nRetry: break here\nIgnore: skip" );
		::MessageBoxA( nullptr, message.data(), "Runtime interrupt", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_APPLMODAL );
	#else
		DebugInterface::_defaultPrintHandler( message.c_str(), message.length() );
	#endif
	}

}
