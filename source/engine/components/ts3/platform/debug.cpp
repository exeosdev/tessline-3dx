
#include "debug.h"

namespace ts3
{

	void DebugInterface::assertionFail( DebugAssertionFailHandlerType pHandler, const FileLocationInfo & pLocationInfo, const char * pConditionStr )
	{
		if( !pHandler )
		{
			pHandler = _defaultAssertionFailHandler;
		}

		pHandler( pLocationInfo, pConditionStr );
	}

	void DebugInterface::interrupt( DebugInterruptHandlerType pHandler, const FileLocationInfo & pLocationInfo )
	{
		if( !pHandler )
		{
			pHandler = _defaultInterruptHandler;
		}

		pHandler( pLocationInfo );
	}

	void DebugInterface::printDebug( DebugPrintHandlerType pHandler, const char* pFormat, ... )
	{
		if( !pHandler )
		{
			pHandler = _defaultPrintHandler;
		}

		const size_t localBufferSize = 4096;
		char localBuffer[localBufferSize];

		size_t printCount = 0;
		char * bufferPtr = &( localBuffer[0] );

		{
			va_list varArgsList;
			va_start( varArgsList, pFormat );

			// This returns the number that *would be written* if buffer was large enough.
			// For a valid case case, it should always smaller than our buffer size. If not,
			// we need to make the buffer capacity a bit bigger.
			printCount = vsnprintf( bufferPtr, localBufferSize, pFormat, varArgsList );

			if( printCount > localBufferSize )
			{
				// Just signal via debug break. We don't need any runtime
				// error handling - this is used only in debug mode anyway.
				TS3_PCL_DEBUG_BREAK();
			}

			va_end( varArgsList );
		}

		pHandler( localBuffer, printCount );
	}

	void DebugInterface::_defaultAssertionFailHandler( const FileLocationInfo & pLocationInfo, const char * pConditionStr )
	{
		std::string message{ "\n|> Debug assertion failed!" };
		message.append( "\n|> " );
		message.append( pLocationInfo.toString() );
		message.append( "\n|> [ " );
		message.append( pConditionStr );
		message.append( " ] is false\n" );

		TS3_PCL_DEBUG_OUTPUT( message.c_str() );
		TS3_PCL_DEBUG_BREAK();
	}

	void DebugInterface::_defaultInterruptHandler( const FileLocationInfo & pLocationInfo )
	{
		std::string message{ "\n|> Debug interrupt triggered!" };
		message.append( "\n|> " );
		message.append( pLocationInfo.toString() );
		message.append( "\n" );

		TS3_PCL_DEBUG_OUTPUT( message.c_str() );
		TS3_PCL_DEBUG_BREAK();
	}

	void DebugInterface::_defaultPrintHandler( const char * pText, size_t pLength )
	{
		TS3_PCL_DEBUG_OUTPUT( pText );
	}

}
