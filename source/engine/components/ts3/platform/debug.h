
#pragma once

#ifndef __TS3_PLATFORM_DEBUG_H__
#define __TS3_PLATFORM_DEBUG_H__

#include "fileLocationInfo.h"

namespace ts3
{

	using DebugAssertionFailHandlerType = void(*)( const FileLocationInfo &, const char * );
	using DebugInterruptHandlerType = void(*)( const FileLocationInfo & );
	using DebugPrintHandlerType = void(*)( const char *, size_t );

	// The three macros defined below allow to override the default handlers used for standard debug macros.
	// Obviously, this works at build time and is useful when building a customized version of the engine.
	// Also, since this doesn't change the already built engine modules, re-definition can enable different
	// behaviour for the target app/game while keeping the default one for the engine itself.
	// - _ts3DebugAssertionFailHandler must match the signature of DebugAssertionFailHandlerType
	// - _ts3DebugInterruptHandler must match the signature of DebugInterruptHandlerType
	// - _ts3DebugPrintHandler must match the signature of DebugPrintHandlerType

	#if !defined( _ts3DebugAssertionFailHandler )
	// Custom handler for the assertion fail macro.
	#  define _ts3DebugAssertionFailHandler nullptr
	#endif

	#if !defined( _ts3DebugInterruptHandler )
	// Custom handler for the interrupt macro.
	#  define _ts3DebugInterruptHandler nullptr
	#endif

	#if !defined( _ts3DebugPrintHandler )
	// Custom handler for the print macro.
	#  define _ts3DebugPrintHandler nullptr
	#endif


	/// @brief
	class DebugInterface
	{
	public:
		/// @brief Called when an interrupt is triggered. Breaks execution using provided mechanism (default or user-specified).
		static void assertionFail( DebugAssertionFailHandlerType pHandler, const FileLocationInfo & pLocationInfo, const char * pConditionStr );

		/// @brief Called when an interrupt is triggered. Breaks execution using provided mechanism (default or user-specified).
		static void interrupt( DebugInterruptHandlerType pHandler, const FileLocationInfo & pLocationInfo );

		/// @brief Handles the print forwarding and vararg handling.
		static void printDebug( DebugPrintHandlerType pHandler, const char* pFormat, ... );

	private:
		// Default assertion handler: prints assertion info using TS3_PCL_DEBUG_OUTPUT() and then triggers an interrupt with TS3_PCL_DEBUG_BREAK().
		static void _defaultAssertionFailHandler( const FileLocationInfo & pLocationInfo, const char * pConditionStr );

		// Default interrupt handler: triggers an interrupt with TS3_PCL_DEBUG_BREAK().
		static void _defaultInterruptHandler( const FileLocationInfo & pLocationInfo );

		// Default print handler: prints specified text with TS3_PCL_DEBUG_OUTPUT().
		static void _defaultPrintHandler( const char * pText, size_t pLength );
	};


	/// @brief Issues a debug interrupt if provided condition is not met. Usually implemented via { asm int 3 } or its equivalent.
	/// If _ts3DebugAssertionFailHandler is defined, provided handler is used instead.
	#define ts3DebugAssert( pCondition ) \
        { \
            if(!(pCondition)) \
            { \
                ::ts3::DebugInterface::assertionFail( _ts3DebugAssertionFailHandler, ts3CurrentFileLocationInfo(), #pCondition ); \
            } \
        }

	/// @brief Issues a debug interrupt if provided condition is not met. Interruption is triggered only once, at the first condition failure.
	/// If _ts3DebugAssertionFailHandler is defined, provided handler is used instead.
	#define ts3DebugAssertOnce( pCondition ) \
		{ \
			static bool cFlag = false; \
			if( !cFlag && !(pCondition) ) \
			{ \
				::ts3::DebugInterface::assertionFail( _ts3DebugAssertionFailHandler, ts3CurrentFileLocationInfo(), #pCondition ); \
				cFlag = true; \
			} \
		}

	/// @brief Issues a debug interrupt. Usually implemented via { asm int 3 } or its equivalent.
	/// If _ts3DebugInterruptHandler is defined, provided handler is used instead.
	#define ts3DebugInterrupt() \
        { \
            ::ts3::DebugInterface::interrupt( _ts3DebugInterruptHandler, ts3CurrentFileLocationInfo() ); \
        }

	/// @brief Issues a debug interrupt. Interruption is triggered only once, at the first condition failure.
	/// If _ts3DebugInterruptHandler is defined, provided handler is used instead.
	#define ts3DebugInterruptOnce() \
		{ \
			static bool cFlag = false; \
			if( !cFlag ) \
			{ \
				::ts3::DebugInterface::interrupt( _ts3DebugInterruptHandler, ts3CurrentFileLocationInfo() ); \
				cFlag = true; \
			} \
		}

	/// @brief Prints formatted output to the default debug output stream.
	/// If _ts3DebugPrintHandler is defined, provided handler is used instead.
	#define ts3DebugOutput( pFormat, ... ) \
		::ts3::DebugInterface::printDebug( _ts3DebugPrintHandler, pFormat, __VA_ARGS__  )


	/// @brief Similar to ts3DebugAssert, but uses explicitly specified handler to handle a condition failure.
	/// If _ts3DebugAssertionFailHandler is defined, provided handler is used instead.
	#define ts3DebugAssertEx( pHandler, pCondition ) \
        { \
	        if(!(pCondition)) \
	        { \
	            ::ts3::DebugInterface::assertionFail( pHandler, ts3CurrentFileLocationInfo(), #pCondition ); \
	        } \
        }

	/// @brief Similar to ts3DebugAssertOnce, but uses explicitly specified handler to handle a condition failure.
	/// If _ts3DebugAssertionFailHandler is defined, provided handler is used instead.
	#define ts3DebugAssertOnceEx( pHandler, pCondition ) \
		{ \
			static bool cFlag = false; \
			if( !cFlag && !(pCondition) ) \
			{ \
				::ts3::DebugInterface::assertionFail( pHandler, ts3CurrentFileLocationInfo(), #pCondition ); \
				cFlag = true; \
			} \
		}

	/// @brief Similar to ts3DebugInterrupt, but uses explicitly specified handler to handle a condition failure.
	/// If _ts3DebugInterruptHandler is defined, provided handler is used instead.
	#define ts3DebugInterruptEx( pHandler ) \
        { \
            ::ts3::DebugInterface::interrupt( pHandler, ts3CurrentFileLocationInfo() ); \
        }

	/// @brief Similar to ts3DebugInterruptOnce, but uses explicitly specified handler to handle a condition failure.
	/// If _ts3DebugInterruptHandler is defined, provided handler is used instead.
	#define ts3DebugInterruptOnceEx( pHandler ) \
		{ \
			static bool cFlag = false; \
			if( !cFlag ) \
			{ \
				::ts3::DebugInterface::interrupt( pHandler, ts3CurrentFileLocationInfo() ); \
				cFlag = true; \
			} \
		}

	/// @brief Similar to ts3DebugOutput, but uses explicitly specified handler to handle a condition failure.
	/// If _ts3DebugPrintHandler is defined, provided handler is used instead.
	#define ts3DebugOutputEx( pHandler, pFormat, ... ) \
		::ts3::DebugInterface::printDebug( pHandler, pFormat, __VA_ARGS__  )

}

#endif // __TS3_PLATFORM_DEBUG_H__
