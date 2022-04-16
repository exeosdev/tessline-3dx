
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_POSIX_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_POSIX_COMMON_H__

#include <ts3/system/prerequisites.h>

namespace ts3::system
{

	namespace platform
	{

		const char * posixQueryErrnoStringByCode( int pErrno );

	}

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_POSIX_COMMON_H__
