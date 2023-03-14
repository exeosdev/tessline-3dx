
#pragma once

#ifndef __TS3_CORE_EXCEPTION_UTILS_H__
#define __TS3_CORE_EXCEPTION_UTILS_H__

#include "exception.h"

#define ts3ThrowIfNull( pPointer, pExceptionCode ) \
	if( !pPointer ) { ts3Throw( pExceptionCode ); }

#define ts3ThrowDescIfNull( pPointer, pExceptionCode, pDesc ) \
	if( !pPointer ) { ts3ThrowDesc( pExceptionCode, pDesc ); }

namespace ts3
{
} // namespace ts3

#endif // __TS3_CORE_EXCEPTION_UTILS_H__
