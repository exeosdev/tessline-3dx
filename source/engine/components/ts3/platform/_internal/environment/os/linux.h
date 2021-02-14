
#ifndef  __TS3_PLATFORM_CONFIG_INTERNAL_OS_LINUX_H__
#define  __TS3_PLATFORM_CONFIG_INTERNAL_OS_LINUX_H__

#include <dlfcn.h>
#include <cinttypes>
#include <csignal>

#undef major
#undef minor

#define TS3_PCL_ENV_DEFAULT_PATH_DELIMITER     '/'
#define TS3_PCL_ENV_DEFAULT_PATH_DELIMITER_STR "/"
#define TS3_PCL_ENV_DYNAMIC_LIBRARY_PREFIX     "lib"
#define TS3_PCL_ENV_DYNAMIC_LIBRARY_EXTENSION  ".so"

#endif /*  __TS3_PLATFORM_CONFIG_INTERNAL_OS_LINUX_H__ */
