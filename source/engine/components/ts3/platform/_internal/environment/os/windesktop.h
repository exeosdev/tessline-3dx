
#ifndef  __TS3_PLATFORM_CONFIG_INTERNAL_OS_WIN32_H__
#define  __TS3_PLATFORM_CONFIG_INTERNAL_OS_WIN32_H__

#include <direct.h>
#include <io.h>
#include <Windows.h>
#include <wrl/client.h>

template <typename TVal>
using ComPtr = Microsoft::WRL::ComPtr<TVal>;

#undef ChangeDisplaySettings
#undef CreateFile
#undef CreateWindow
#undef DispatchMessage
#undef EnumDisplayDevices
#undef FindResource
#undef GetClassInfo
#undef GetFreeSpace
#undef GetMessage
#undef GetObject
#undef LoadImage
#undef LoadLibrary
#undef MessageBox
#undef PeekMessage
#undef PostMessage
#undef RegisterClass
#undef RegisterClassEx
#undef SendMessage
#undef UnregisterClass
#undef Yield

#define TS3_PCL_ENV_DEFAULT_PATH_DELIMITER          '\\'
#define TS3_PCL_ENV_DEFAULT_PATH_DELIMITER_STR      "\\"
#define TS3_PCL_ENV_NON_STANDARD_PATH_DELIMITER     '/'
#define TS3_PCL_ENV_NON_STANDARD_PATH_DELIMITER_STR "/"
#define TS3_PCL_ENV_DYNAMIC_LIBRARY_PREFIX          ""
#define TS3_PCL_ENV_DYNAMIC_LIBRARY_EXTENSION       ".dll"

#endif /*  __TS3_PLATFORM_CONFIG_INTERNAL_OS_WIN32_H__ */
